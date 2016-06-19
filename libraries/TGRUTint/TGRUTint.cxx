#include "Globals.h"
#include "TGRUTint.h"

#include <algorithm>
#include <fstream>
#include <string>

#include <pwd.h>

#include "Getline.h"
#include "TClass.h"
#include "TCutG.h"
#include "TFile.h"
#include "TInterpreter.h"
#include "TMethodCall.h"
#include "TPython.h"
#include "TROOT.h"
#include "TString.h"
#include "TThread.h"
#include "TTree.h"
#include "TUnixSystem.h"
#include "TVirtualPad.h"

#include "GRootCommands.h"
#include "GRootGuiFactory.h"
#include "GValue.h"
#include "GrutNotifier.h"
#include "TBuildingLoop.h"
#include "TChainLoop.h"
#include "TChannel.h"
#include "TDataLoop.h"
#include "TDetectorEnv.h"
#include "TFilterLoop.h"
#include "TGRUTOptions.h"
#include "TGRUTUtilities.h"
#include "THistogramLoop.h"
#include "TInverseMap.h"
#include "TMultiRawFile.h"
#include "TOrderedRawFile.h"
#include "TRawSource.h"
#include "TSequentialRawFile.h"
#include "TTerminalLoop.h"
#include "TUnpackingLoop.h"
#include "TWriteLoop.h"

ClassImp(TGRUTint)

TGRUTint *TGRUTint::fTGRUTint = NULL;
TEnv    *TGRUTint::fGRUTEnv  = NULL;


TGRUTint *TGRUTint::instance(int argc,char** argv, void *options, int numOptions, bool noLogo, const char *appClassName) {
  if(!fTGRUTint) {
    fTGRUTint = new TGRUTint(argc,argv,options,numOptions,true,appClassName);
    fTGRUTint->Init();
  }
  return fTGRUTint;
}


TGRUTint::TGRUTint(int argc, char **argv,void *options, Int_t numOptions, Bool_t noLogo,const char *appClassName)
  :TRint(appClassName, &argc, argv, options, numOptions,noLogo),
   fKeepAliveTimer(NULL), main_thread_id(std::this_thread::get_id()), fIsTabComplete(false),
   fAllowedToTerminate(true) {

  fGRUTEnv = gEnv;
  GetSignalHandler()->Remove();
  TGRUTInterruptHandler *ih = new TGRUTInterruptHandler();
  ih->Add();

  fRootFilesOpened = 0;
  fRawFilesOpened  = 0;
  fHistogramLoop = 0;
  fDataLoop = 0;
  fChainLoop = 0;

  SetPrompt("GRizer [%d] ");
  TGRUTOptions::Get(argc, argv);
}


TGRUTint::~TGRUTint() {
  if(fKeepAliveTimer) {
    delete fKeepAliveTimer;
  }
}


void TGRUTint::Init() {

  if(TGRUTOptions::Get()->ShouldExit()){
    Terminate();
    return;
  }


  std::string grutpath = getenv("GRUTSYS");
  gInterpreter->AddIncludePath(Form("%s/include",grutpath.c_str()));

  ApplyOptions();
}

void TGRUTint::SplashPopNWait(bool flag) {
  //PopupLogo(false);
  //WaitLogo();
}


/*********************************/

bool TGRUTInterruptHandler::Notify() {
  static int timespressed  = 0;
  timespressed++;
  if(timespressed>3) {
    printf("\n" DRED BG_WHITE  "   No you shutup! " RESET_COLOR "\n"); fflush(stdout);
    exit(1);
  }
  printf("\n" DRED BG_WHITE  "   Control-c was pressed.   " RESET_COLOR "\n"); fflush(stdout);
  TGRUTint::instance()->Terminate();
  return true;
}

// Copied from http://stackoverflow.com/a/24315631
std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
  size_t start_pos = 0;
  while((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
  }
  return str;
}

void TGRUTint::ApplyOptions() {
  TGRUTOptions* opt = TGRUTOptions::Get();

  bool missing_raw_file = !all_files_exist(opt->RawInputFiles());

  if(!false) { //this can be change to something like, if(!ClassicRoot)
     LoadGRootGraphics();
  }

  fKeepAliveTimer = new TTimer("",1000);
  fKeepAliveTimer->Start();


  TDetectorEnv::Get(opt->DetectorEnvironment().c_str());

  if(opt->S800InverseMapFile().length()) {
    TInverseMap::Get(opt->S800InverseMapFile().c_str());
  }

  for(auto filename : opt->RootInputFiles()) {
    // this will populate gChain if able.
    //   TChannels from the root file will be loaded as file is opened.
    //   GValues from the root file will be loaded as file is opened.
    OpenRootFile(filename);
  }

  //if I am passed any calibrations, lets load those, this
  //will overwrite any with the same address previously read in.
  if(opt->CalInputFiles().size()) {
    for(auto cal_filename : opt->CalInputFiles()) {
      TChannel::ReadCalFile(cal_filename.c_str());
    }
  }
  if(opt->ValInputFiles().size()) {
    for(auto val_filename : opt->ValInputFiles()) {
      GValue::ReadValFile(val_filename.c_str());
    }
  }


  if(opt->StartGUI()){
    StartGUI();
  }

  //ok now, if told not to sort open any raw files as _data# (rootish like??)
  if(opt->RawInputFiles().size() && !opt->SortRaw()) {
    for(unsigned int x=0;x<opt->RawInputFiles().size();x++) {
      OpenRawFile(opt->RawInputFiles().at(x));
    }
  }

  // Sets up all the various loops for data analysis.
  // Note: Assumes that gChain has already been loaded.
  SetupPipeline();

  for(auto& filename : opt->MacroInputFiles()){
    RunMacroFile(filename);
  }

  if(opt->ExitAfterSorting()){
    while(StoppableThread::AnyThreadRunning()){
      std::this_thread::sleep_for(std::chrono::seconds(1));

      // We need to process events in case a different thread is asking for a file to be opened.
      // However, if there is no stdin, ProcessEvents() will call Terminate().
      // This prevents the terminate from taking effect while in this context.
      fAllowedToTerminate = false;
      gSystem->ProcessEvents();
      fAllowedToTerminate = true;

      std::cout << "\r" << StoppableThread::AnyThreadStatus() << std::flush;
    }
    std::cout << std::endl;

    int exit_status = missing_raw_file ? 1 : 0;
    this->Terminate(exit_status);
  }
}

TFile* TGRUTint::OpenRootFile(const std::string& filename, Option_t* opt){
  TString sopt(opt);
  sopt.ToLower();

  // Check if the file is being opened as an online file.
  // This is used by the GUI to know whether it should continually check
  //   for new histograms.
  bool is_online = sopt.Contains("online");
  sopt.ReplaceAll("online","");

  TFile* file = NULL;
  if(sopt.Contains("recreate") ||
     sopt.Contains("new")) {
    // We are being asked to make a new file.
    file = new TFile(filename.c_str(), "RECREATE");
    if(file){
      // Give access to the file inside the interpreter.
      const char* command = Form("TFile* _file%i = (TFile*)%luL",
                                 fRootFilesOpened,
                                 (unsigned long)file);
      //if(!StoppableThread::AnyThreadRunning() && !TGRUTOptions::Get()->ExitAfterSorting()) {
      //if(!fHistogramLoop && !TGRUTOptions::Get()->ExitAfterSorting()) {
        TRint::ProcessLine(command);
	//ProcessLine(command);
      //}
      fRootFilesOpened++;
    } else {
      std::cout << "Could not create " << filename << std::endl;
    }
  } else {

    // Open an already existing file.
    file = new TFile(filename.c_str(), opt);
    if(file){
      // Give access to the file inside the interpreter.
      const char* command = Form("TFile* _%s%i = (TFile*)%luL",
                                 is_online ? "online" : "file",
                                 fRootFilesOpened,
                                 (unsigned long)file);
      //if(!StoppableThread::AnyThreadRunning() && !TGRUTOptions::Get()->ExitAfterSorting()) {
      //if(!fHistogramLoop && !TGRUTOptions::Get()->ExitAfterSorting()) {
        TRint::ProcessLine(command);
	//ProcessLine(command);
      //}
      std::cout << "\tfile " << BLUE << file->GetName() << RESET_COLOR
                <<  " opened as " << BLUE <<  "_file" << fRootFilesOpened << RESET_COLOR <<  std::endl;

      // If EventTree exists, add the file to the chain.
      if(file->FindObjectAny("EventTree")) {
        if(!gChain) { // Should never go in here!!
	  gChain = new TChain("EventTree");
	  gChain->SetNotify(GrutNotifier::Get());
        }
        printf("file %s added to gChain.\n",file->GetName());
        gChain->Add(file->GetName());
      }

      if(file->FindObjectAny("TChannel")){
        file->Get("TChannel");
      }

      if(file->FindObjectAny("GValue")){
        file->Get("GValue");
      }

      fRootFilesOpened++;
    } else {
      std::cout << "Could not open " << filename << std::endl;
    }
  }

  // Keep track of whether the file is online.
  if(is_online){
    file->SetOption("online");
  }

  // Pass the TFile to the python GUI.
  if(file && GUIIsRunning()){
    TPython::Bind(file,"tdir");
    ProcessLine("TPython::Exec(\"window.AddDirectory(tdir)\");");
  }
  return file;
}

void TGRUTint::LoadTCutG(TCutG* cutg) {
  if(GUIIsRunning()) {
    TPython::Bind(cutg, "cutg");
    ProcessLine("TPython::Exec(\"window.LoadCutG(cutg)\");");
  }
}

void TGRUTint::LoadRawFile(std::string filename) {
  if(fDataLoop){
    TRawEventSource* source = new TRawFileIn(filename.c_str());
    if(TGRUTOptions::Get()->TimeSortInput()){
      TOrderedRawFile* ordered_source = new TOrderedRawFile(source);
      ordered_source->SetDepth(TGRUTOptions::Get()->TimeSortDepth());
      source = ordered_source;
    }
    fDataLoop->ReplaceSource(source);
  }
}

void TGRUTint::ResortDataFile() {
  StoppableThread::PauseAll();
  if(fDataLoop){
    fDataLoop->ResetSource();
    for(auto thread : StoppableThread::GetAll()){
      thread->ClearQueue();
    }
  } else if(fChainLoop) {
    fChainLoop->Restart();
    for(auto thread : StoppableThread::GetAll()){
      thread->ClearQueue();
    }

  }
  StoppableThread::ResumeAll();
}



TRawFileIn *TGRUTint::OpenRawFile(const std::string& filename) {
  if(!file_exists(filename.c_str())){
    std::cerr << "File \"" << filename << "\" does not exist" << std::endl;
    return NULL;
  }
  const char* command1 = Form("TGRUTint::instance()->GetListOfRawFiles()->Add(new TRawFileIn(\"%s\"))",filename.c_str());
  ProcessLine(command1);
  const char* command2 = Form("TRawFileIn *_data%i = (TRawFileIn*)TGRUTint::instance()->GetListOfRawFiles()->Last()",fRawFilesOpened);
  ProcessLine(command2);
  TRawFileIn *file = (TRawFileIn*)GetListOfRawFiles()->Last();
  if(file){
    //std::string name = file->GetName();
    //std::replace(name.begin(),name.end(),'_','/');
    std::cout << "\tfile " << BLUE << filename << RESET_COLOR << " opened as "<< BLUE << "_data" <<  fRawFilesOpened << RESET_COLOR << std::endl;
  }
  fRawFilesOpened++;
  return file;
}

void TGRUTint::SetupPipeline() {
  TGRUTOptions* opt = TGRUTOptions::Get();

  // Determining which parts of the pipeline need to be set up.

  bool missing_raw_file = false;
  for(auto& filename : opt->RawInputFiles()) {
    if(!file_exists(filename.c_str())) {
      missing_raw_file = true;
      std::cerr << "File not found: " << filename << std::endl;
    }
  }

  bool has_input_ring = opt->InputRing().length();
  bool has_raw_file = opt->RawInputFiles().size();
  bool self_stopping = opt->ExitAfterSorting();
  bool sort_raw = ( (has_input_ring || has_raw_file) &&
                    !missing_raw_file && opt->SortRaw() );

  bool has_explicit_root_output = opt->OutputFile().length();
  bool filter_data = opt->CompiledFilterFile().length();
  bool raw_filtered_output = opt->OutputFilteredFile().length();

  bool has_input_tree = gChain->GetListOfBranches();
  bool sort_tree = has_input_tree && (opt->MakeHistos() || opt->SortRoot() ||
                                      (has_explicit_root_output && filter_data));

  bool write_root_tree = sort_raw || (sort_tree && filter_data && has_explicit_root_output);
  bool write_histograms = opt->MakeHistos() || sort_tree;

  std::string output_root_file = "temp_tree.root";
  if(opt->OutputFile().length()) {
    output_root_file = opt->OutputFile();
  } else if(opt->RawInputFiles().size() == 1) {
    output_root_file = "run" + get_run_number(opt->RawInputFiles().front()) + ".root";
  } else if(opt->RawInputFiles().size() > 1) {
    output_root_file = ("run" +
                        get_run_number(opt->RawInputFiles().front()) + "-" +
                        get_run_number(opt->RawInputFiles().back()) +
                        ".root");
  } else if(has_input_ring) {
    output_root_file = "ring_tree.root";
  }


  std::string output_hist_file = "temp_hist.root";
  if(opt->OutputHistogramFile().length()) {
    output_hist_file = opt->OutputHistogramFile();
  } else if(sort_raw && opt->RawInputFiles().size() == 1) {
    output_hist_file = "hist" + get_run_number(opt->RawInputFiles().front()) + ".root";
  } else if(sort_raw && opt->RawInputFiles().size() > 1) {
    output_hist_file = ("hist" +
                        get_run_number(opt->RawInputFiles().front()) + "-" +
                        get_run_number(opt->RawInputFiles().back()) +
                        ".root");
  } else if(sort_raw && has_input_ring) {
    output_hist_file = "ring_tree.hist";
  } else if(sort_tree && opt->RootInputFiles().size() == 1) {
    output_hist_file = "hist" + get_run_number(opt->RootInputFiles().front()) + ".root";
  } else if(sort_tree && opt->RootInputFiles().size() > 1) {
    output_hist_file = ("hist" +
                        get_run_number(opt->RootInputFiles().front()) + "-" +
                        get_run_number(opt->RootInputFiles().back()) +
                        ".root");
  }


  // Now, start setting stuff up

  std::vector<TFile*> cuts_files;
  for(auto filename : opt->CutsInputFiles()) {
    TFile* tfile = OpenRootFile(filename);
    cuts_files.push_back(tfile);
  }

  // No need to set up all the loops if we are just opening the interpreter.
  if(!sort_raw && !sort_tree) {
    return;
  }

  std::shared_ptr<ThreadsafeQueue<TUnpackedEvent*> > current_queue = nullptr;

  //next most important thing, if given a raw file && NOT told to not sort!
  if(sort_raw) {
    TRawEventSource* source = OpenRawSource();
    fDataLoop = TDataLoop::Get("1_input_loop",source);
    fDataLoop->SetSelfStopping(self_stopping);

    TBuildingLoop* build_loop = TBuildingLoop::Get("2_build_loop");
    build_loop->SetBuildWindow(opt->BuildWindow());
    build_loop->InputQueue() = fDataLoop->OutputQueue();

    TUnpackingLoop* unpack_loop = TUnpackingLoop::Get("3_unpack");
    unpack_loop->InputQueue() = build_loop->OutputQueue();
    current_queue = unpack_loop->OutputQueue();

  } else if(sort_tree) {
    fChainLoop = TChainLoop::Get("1_chain_loop",gChain);
    fChainLoop->SetSelfStopping(self_stopping);
    current_queue = fChainLoop->OutputQueue();
  }

  if(filter_data) {
    TFilterLoop* filter_loop = TFilterLoop::Get("4_filter_loop");
    if(raw_filtered_output) {
      filter_loop->OpenRawOutputFile(opt->OutputFilteredFile());
    }
    filter_loop->InputQueue() = current_queue;
    current_queue = filter_loop->OutputQueue();
  }

  if(write_root_tree) {
    TWriteLoop* write_loop = TWriteLoop::Get("5_write_loop", output_root_file);
    write_loop->InputQueue() = current_queue;
    current_queue = write_loop->OutputQueue();
  }

  if(write_histograms) {
    fHistogramLoop = THistogramLoop::Get("6_hist_loop");
    fHistogramLoop->SetOutputFilename(output_hist_file);
    for(auto cut_file : cuts_files) {
      fHistogramLoop->AddCutFile(cut_file);
    }
    fHistogramLoop->InputQueue() = current_queue;
    current_queue = fHistogramLoop->OutputQueue();
  }

  TTerminalLoop* terminal_loop = TTerminalLoop::Get("7_terminal_loop");
  terminal_loop->InputQueue() = current_queue;

  StoppableThread::ResumeAll();
}

TRawEventSource* TGRUTint::OpenRawSource() {
  TGRUTOptions* opt = TGRUTOptions::Get();

  bool has_input_ring = opt->InputRing().length();
  bool has_raw_file = opt->RawInputFiles().size();

  TRawEventSource* source = NULL;

  if(has_input_ring) {
    // Open a source from a ring.
    source = TRawEventSource::EventSource(opt->InputRing().c_str(),
                                          true, true,
                                          opt->DefaultFileType());

  } else if(has_raw_file && opt->SortMultiple()){
    // Open multiple files, read from all at the same time.
    TMultiRawFile* multi_source = new TMultiRawFile();
    for(auto& filename : opt->RawInputFiles()){
      multi_source->AddFile(new TRawFileIn(filename.c_str()));
    }
    source = multi_source;

  } else if(opt->RawInputFiles().size() > 1 && !opt->SortMultiple()){
    // Open multiple files, read from each one at a a time.
    TSequentialRawFile* seq_source = new TSequentialRawFile();
    for(auto& filename : opt->RawInputFiles()){
      seq_source->Add(new TRawFileIn(filename.c_str()));
    }
    source = seq_source;

  } else {
    // Open a single file.
    std::string filename = opt->RawInputFiles().at(0);
    if(file_exists(filename.c_str())){
      source = new TRawFileIn(filename.c_str());
    }
  }

  if(opt->TimeSortInput()){
    TOrderedRawFile* ordered_source = new TOrderedRawFile(source);
    ordered_source->SetDepth(opt->TimeSortDepth());
    source = ordered_source;
  }

  return source;
}

void TGRUTint::RunMacroFile(const std::string& filename){
  if(!file_exists(filename.c_str())){
    std::cerr << "File \"" << filename << "\" does not exist" << std::endl;
    return;
  }

  const char* command = Form(".x %s", filename.c_str());
  ProcessLine(command);
}

void TGRUTint::ResetAllHistograms() {
  if(fHistogramLoop){
    fHistogramLoop->ClearHistograms();
  }
}

Int_t TGRUTint::TabCompletionHook(char* buf, int* pLoc, std::ostream& out){
  fIsTabComplete = true;
  auto result = TRint::TabCompletionHook(buf, pLoc, out);
  fIsTabComplete = false;
  return result;
}


Long_t TGRUTint::ProcessLine(const char* line, Bool_t sync,Int_t *error) {
  // If you print while fIsTabComplete is true, you will break tab complete.
  // Any diagnostic print statements should be done after this if statement.

  sync = false;

  if(fIsTabComplete){
    long res = TRint::ProcessLine(line, sync, error);
    return res;
  }
  TString sline(line);
  if(!sline.Length()){
    return 0;
  }
  sline.ReplaceAll("TCanvas","GCanvas");

  if(std::this_thread::get_id() != main_thread_id){
    return DelayedProcessLine(line);
  }

  if(!sline.CompareTo("clear")) {
    long result = TRint::ProcessLine(".! clear");
    return result;
  } else if(!sline.CompareTo("xterm")) {
    long result = TRint::ProcessLine(".! xterm &");
    return result;
  } else if(sline.BeginsWith("vim ")) {
    sline.ReplaceAll("vim ",".! vim ");
  } else if(sline.BeginsWith("emacs ")) {
    sline.ReplaceAll("emacs ",".! emacs -nw ");
  }

  long result =  TRint::ProcessLine(sline.Data(),sync,error);

  return result;
}


void TGRUTint::Terminate(Int_t status){
  //std::cout << "Trying to terminate..." << std::endl;
  if(!fAllowedToTerminate){
    //std::cout << "SQUASHED!" << std::endl;
    return;
  }
  StoppableThread::StopAllClean();

  //if(GUIIsRunning()){
  //  TPython::Exec("on_close()");
  //}

  //Be polite when you leave.
  printf(DMAGENTA "\nbye,bye\t" DCYAN "%s" RESET_COLOR  "\n",
         getpwuid(getuid())->pw_name);

  if((clock()%60) == 0){
    printf("DING!");
    fflush(stdout);
    gSystem->Sleep(500);
    printf("\r              \r");
    fflush(stdout);
  }

  TChannel::DeleteAllChannels();
  TRint::Terminate(status);
}

void TGRUTint::LoadGRootGraphics() {
  GRootGuiFactory::Init();
}

//   These variables are to be accessed only from DelayedProcessLine
// and DelayedProcessLine_Action, nowhere else.
//   These are used to pass information between the two functions.
// DelayedProcessLine_Action() should ONLY be called from a TTimer running
// on the main thread.  DelayedProcessLine may ONLY be called
// from inside ProcessLine().
namespace {
  std::mutex g__CommandListMutex;
  std::mutex g__ResultListMutex;
  std::mutex g__CommandWaitingMutex;
  std::condition_variable g__NewResult;

  std::string g__LineToProcess;
  bool g__ProcessingNeeded;

  Long_t g__CommandResult;
  bool g__CommandFinished;
}

Long_t TGRUTint::DelayedProcessLine(std::string command){
  std::lock_guard<std::mutex> any_command_lock(g__CommandWaitingMutex);

  g__LineToProcess = command;
  g__CommandFinished = false;
  g__ProcessingNeeded = true;
  TTimer::SingleShot(0,"TGRUTint",this,"DelayedProcessLine_Action()");

  std::unique_lock<std::mutex> lock(g__ResultListMutex);
  while(!g__CommandFinished){
    g__NewResult.wait(lock);
  }

  return g__CommandResult;
}


void TGRUTint::DelayedProcessLine_Action(){
  std::string message;
  {
    std::lock_guard<std::mutex> lock(g__CommandListMutex);
    if(!g__ProcessingNeeded){
      return;
    }
    message = g__LineToProcess;
  }

  Long_t result = this->ProcessLine(message.c_str());
  Getlinem(EGetLineMode::kInit,((TRint*)gApplication)->GetPrompt());

  {
    std::lock_guard<std::mutex> lock(g__ResultListMutex);
    g__CommandResult = result;
    g__CommandFinished = true;
    g__ProcessingNeeded = false;
  }

  g__NewResult.notify_one();
}
