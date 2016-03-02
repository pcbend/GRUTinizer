#include "Globals.h"
#include "TGRUTint.h"

#include <algorithm>
#include <fstream>
#include <string>

#include <pwd.h>

#include <Getline.h>
#include <TClass.h>
#include <TFile.h>
#include <TInterpreter.h>
#include <TPython.h>
#include <TROOT.h>
#include <TString.h>
#include <TTree.h>
#include <TUnixSystem.h>

#include <TCutG.h>
#include <TThread.h>
#include <TMethodCall.h>
#include <TVirtualPad.h>

#include "GRootGuiFactory.h"

#include "TChannel.h"
#include "GValue.h"
#include "TDetectorEnv.h"

#include "TGRUTOptions.h"
#include "TOrderedRawFile.h"
#include "TRawSource.h"
#include "TMultiRawFile.h"
#include "TSequentialRawFile.h"

#include "GrutNotifier.h"
#include "TGRUTUtilities.h"
#include "GRootCommands.h"

#include "TDataLoop.h"
#include "TBuildingLoop.h"
#include "TUnpackingLoop.h"
#include "TWriteLoop.h"
#include "TChainLoop.h"
#include "THistogramLoop.h"

#include "TS800.h"

//extern "C" G__value G__getitem(const char* item);
//#include "FastAllocString.h"
//char* G__valuemonitor(G__value buf, G__FastAllocString& temp);

//extern void PopupLogo(bool);
//extern void WaitLogo();


//TChain *gChain = NULL;

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
   main_thread_id(std::this_thread::get_id()), fIsTabComplete(false),
   fAllowedToTerminate(true) {

  fGRUTEnv = gEnv;
  GetSignalHandler()->Remove();
  TGRUTInterruptHandler *ih = new TGRUTInterruptHandler();
  ih->Add();

  fChain = 0;
  fRootFilesOpened = 0;
  fRawFilesOpened  = 0;
  fHistogramLoop = 0;
  fDataLoop = 0;
  fChainLoop = 0;

  SetPrompt("GRizer [%d] ");
  TGRUTOptions::Get(argc, argv);
}


TGRUTint::~TGRUTint() { }


void TGRUTint::Init() {

  if(TGRUTOptions::Get()->ShouldExit()){
    Terminate();
    return;
  }


  std::string grutpath = getenv("GRUTSYS");
  gInterpreter->AddIncludePath(Form("%s/include",grutpath.c_str()));

  LoadDetectorClasses();

  ApplyOptions();
}

void TGRUTint::SplashPopNWait(bool flag) {
  //PopupLogo(false);
  //WaitLogo();
}

void TGRUTint::LoadDetectorClasses() {
  if(!gROOT->LoadClass("TGretina") ||
     !gROOT->LoadClass("TGretinaHit") ||
     !gROOT->LoadClass("std::vector<TGretinaHit>") ||
     !gROOT->LoadClass("TS800") ||

     !gROOT->LoadClass("TSega") ||
     !gROOT->LoadClass("TJanus")
    //gROOT->LoadClass("TPhosWall",false);
  ){
    std::cout << "Could not load all GRUT classes" << std::endl;
  }

}


/*********************************/

bool TGRUTInterruptHandler::Notify() {
  static int timespressed  = 0;
  timespressed++;
  if(timespressed>3) {
    printf("\n" DRED BG_WHITE  "   No you shutup! " RESET_COLOR "\n");
    exit(1);
  }
  printf("\n" DRED BG_WHITE  "   Control-c was pressed.   " RESET_COLOR "\n");
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

  if(!false) { //this can be change to something like, if(!ClassicRoot)
     LoadGRootGraphics();
  }


  TDetectorEnv::Get(opt->DetectorEnvironment().c_str());

  if(opt->S800InverseMapFile().length()) {
    TS800::ReadInverseMap(opt->S800InverseMapFile().c_str());
  }

  //next, if given a root file and NOT told to sort it..
  if(opt->RootInputFiles().size()){
    for(unsigned int x=0;x<opt->RootInputFiles().size();x++) {
      OpenRootFile(opt->RootInputFiles().at(x));
      // this we creat and populate gChain if able.
      //   TChannels from the root file will be loaded as file is opened.
      //   GValues from the root file will be loaded as file is opened.
    }
  }

  //if I am passed any calibrations, lets load those, this
  //will over write any wit the same address previously read in.
  if(opt->CalInputFiles().size()) {
    for(unsigned int x=0;x<opt->CalInputFiles().size();x++) {
      TChannel::ReadCalFile(opt->CalInputFiles().at(x).c_str());
    }
  }
  if(opt->ValInputFiles().size()) {
    for(unsigned int x=0;x<opt->ValInputFiles().size();x++) {
      GValue::ReadValFile(opt->ValInputFiles().at(x).c_str());
    }
  }


  if(opt->StartGUI()){
    StartGUI();
  }

  //next most important thing, if given a raw file && NOT told to not sort!
  if((opt->InputRing().length() || opt->RawInputFiles().size())
     && opt->SortRaw()) {

    TRawEventSource* source = NULL;
    if(opt->InputRing().length()) {
      // Open a source from a ring.
      source = new TRawEventRingSource(opt->InputRing(),
                                       opt->DefaultFileType());

    } else if(opt->RawInputFiles().size() > 1 && opt->SortMultiple()){
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
      source = new TRawFileIn(filename.c_str());
    }

    if(opt->TimeSortInput()){
      std::cout << "\n\nI am time sorted\n\n" << std::endl;
      TOrderedRawFile* ordered_source = new TOrderedRawFile(source);
      ordered_source->SetDepth(opt->TimeSortDepth());
      source = ordered_source;
    }

    fDataLoop = TDataLoop::Get("1_input_loop",source);
    // If we are not exiting automatically, keep the data loop alive
    //   so that we can open a new file.
    if(!opt->ExitAfterSorting()){
      fDataLoop->SetSelfStopping(false);
    }

    fDataLoop->Resume();


    TBuildingLoop *boop = TBuildingLoop::Get("2_build_loop",fDataLoop);
    boop->SetBuildWindow(opt->BuildWindow());
    boop->Resume();
    TUnpackingLoop *uoop1 = TUnpackingLoop::Get("3_unpack1",boop);
    uoop1->Resume();
    //TUnpackingLoop *uoop2 = TUnpackingLoop::Get("unpack2",boop);
    //uoop2->Resume();
    // TUnpackingLoop *uoop3 = TUnpackingLoop::Get("unpack3",boop);
    // uoop3->Resume();


    // Determine output file names
    std::string rootoutfile = "temp_tree.root";
    std::string histoutfile = "temp_hist.root";
    if(opt->RawInputFiles().size() > 0){
      std::string filename = opt->RawInputFiles()[0];
      rootoutfile = "run" + get_run_number(filename) + ".root";
      histoutfile = "hist" + get_run_number(filename) + ".root";
    } else {
      rootoutfile = "ring_tree.root";
      histoutfile = "ring_hist.root";
    }

    if(opt->OutputFile().length()) {
      rootoutfile = opt->OutputFile();
    }
    if(opt->OutputHistogramFile().length()) {
      histoutfile = opt->OutputHistogramFile();
    }

    TWriteLoop* woop = TWriteLoop::Get("4_write_loop", rootoutfile);
    woop->Connect(uoop1);
    //woop->Connect(uoop2);

    if(TGRUTOptions::Get()->MakeHistos()){
      fHistogramLoop = THistogramLoop::Get("5_hist_loop");
      fHistogramLoop->SetOutputFilename(histoutfile);
      woop->AttachHistogramLoop(fHistogramLoop);
      fHistogramLoop->Resume();
    }
    woop->Resume();
  } else if(opt->RawInputFiles().size() && !opt->SortRaw()) {
    //ok now, if told not to sort open any raw files as _data# (rootish like??)
    for(unsigned int x=0;x<opt->RawInputFiles().size();x++) {
      OpenRawFile(opt->RawInputFiles().at(x));
    }
  }


  //next, if given a root file and told to sort it.
  //TChainLoop* coop = NULL;
  if(gChain && (opt->MakeHistos() || opt->SortRoot()) ){
    printf("Attempting to sort root files.\n");
    fChainLoop = TChainLoop::Get("1_chain_loop",gChain);
    if(!opt->ExitAfterSorting()){
      fChainLoop->SetSelfStopping(false);
    }
    fHistogramLoop = THistogramLoop::Get("2_hist_loop");
    gChain->GetEntry(0);
    std::string histoutfile = "hist" + get_run_number(gChain->GetCurrentFile()->GetName()) + ".root";
    if(opt->OutputHistogramFile().length()) {
      histoutfile = opt->OutputHistogramFile();
    } else {
      if(opt->RootInputFiles().size()==1) {
        histoutfile = "hist" + get_run_number(opt->RootInputFiles().at(0)) + ".root";
      } else if(opt->RootInputFiles().size()>1){
        histoutfile = "hist" + get_run_number(opt->RootInputFiles().at(0)) + "-"
                             + get_run_number(opt->RootInputFiles().at(opt->RootInputFiles().size()-1))
                             + ".root";
      }
    }
    fHistogramLoop->SetOutputFilename(histoutfile);
    fChainLoop->AttachHistogramLoop(fHistogramLoop);
    fHistogramLoop->Resume();
    fChainLoop->Resume();
  }

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

    this->Terminate();
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
      ProcessLine(command);
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
      ProcessLine(command);
      std::cout << "\tfile " << BLUE << file->GetName() << RESET_COLOR
                <<  " opened as " << BLUE <<  "_file" << fRootFilesOpened << RESET_COLOR <<  std::endl;

      // If EventTree exists, add the file to the chain.
      if(file->FindObjectAny("EventTree")) {
        if(!gChain) {
          gChain = new TChain("EventTree");
          gChain->SetNotify(GrutNotifier::Get());
        }
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
  if(file && TGRUTOptions::Get()->StartGUI()){
    TPython::Bind(file,"tdir");
    ProcessLine("TPython::Exec(\"window.AddDirectory(tdir)\");");
  }
  return file;
}

void TGRUTint::LoadTCutG(TCutG* cutg) {
  if(TGRUTOptions::Get()->StartGUI()) {
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
  }

  long result =  TRint::ProcessLine(sline.Data(),sync,error);

  return result;
}


void TGRUTint::Terminate(Int_t status){
  if(!fAllowedToTerminate){
    return;
  }
  StoppableThread::StopAllClean();

  //if(TGRUTOptions::Get()->StartGUI()){
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
