#include "Globals.h"
#include "TGRUTint.h"

#include <fstream>
#include <string>
#include <algorithm>

#include <Getline.h>
#include <TClass.h>
#include <TFile.h>
#include <TInterpreter.h>
#include <TPython.h>
#include <TROOT.h>
#include <TString.h>
#include <TTree.h>
#include <TUnixSystem.h>

#include <TThread.h>
#include <TMethodCall.h>
#include <TVirtualPad.h>

#include "GRootGuiFactory.h"

#include "TChannel.h"
#include "TDetectorEnv.h"

#include "TGRUTOptions.h"
#include "TRawSource.h"

#include "TGRUTUtilities.h"

#include "TChannel.h"

#include "TS800.h"

#include "TDataLoop.h"
#include "TBuildingLoop.h"
#include "TUnpackingLoop.h"
#include "TWriteLoop.h"
#include "TChainLoop.h"
#include "THistogramLoop.h"

#include "TSega.h"

//extern "C" G__value G__getitem(const char* item);
//#include "FastAllocString.h"
//char* G__valuemonitor(G__value buf, G__FastAllocString& temp);

//extern void PopupLogo(bool);
//extern void WaitLogo();


TChain *gChain = NULL;

ClassImp(TGRUTint)

TGRUTint *TGRUTint::fTGRUTint = NULL;
TEnv    *TGRUTint::fGRUTEnv  = NULL;
//TObject *gResponse = NULL;


TGRUTint *TGRUTint::instance(int argc,char** argv, void *options, int numOptions, bool noLogo, const char *appClassName) {
  if(!fTGRUTint) {
    fTGRUTint = new TGRUTint(argc,argv,options,numOptions,true,appClassName);
    fTGRUTint->Init();
  }
  return fTGRUTint;
}


TGRUTint::TGRUTint(int argc, char **argv,void *options, Int_t numOptions, Bool_t noLogo,const char *appClassName)
  :TRint(appClassName, &argc, argv, options, numOptions,noLogo),
   fIsTabComplete(false), main_thread_id(std::this_thread::get_id()) {
   //fCommandServer(NULL), fGuiTimer(NULL), fCommandTimer(NULL),  fIsTabComplete(false) {

  fGRUTEnv = gEnv;
  GetSignalHandler()->Remove();
  TGRUTInterruptHandler *ih = new TGRUTInterruptHandler();
  ih->Add();
  //TH1::SetDefaultSumw2();

  fChain = 0;
  fRootFilesOpened = 0;
  fRawFilesOpened  = 0;


  SetPrompt("GRizer [%d] ");
  TGRUTOptions::Get(argc, argv);
}


TGRUTint::~TGRUTint() {
  //if(fCommandTimer){
  //  delete fCommandTimer;
  //}
  //if(fGuiTimer){
  //  delete fGuiTimer;
  //}
}


void TGRUTint::Init() {

  if(TGRUTOptions::Get()->ShouldExit()){
    Terminate();
    return;
  }


  std::string grutpath = getenv("GRUTSYS");
  gInterpreter->AddIncludePath(Form("%s/include",grutpath.c_str()));
  //gSystem->AddIncludePath(Form("-I%s/include",grutpath.c_str()));
  //gSystem->AddDynamicPath(Form("-%s/libraries",grutpath.c_str()));

  LoadDetectorClasses();

//  if(TGRUTOptions::Get()->ShowLogo()){
//    std::thread splashscreen(&TGRUTint::SplashPopNWait,this,false);
//    splashscreen.detach();
//  }

  // if(TGRUTOptions::Get()->ShowLogo()){
  //   PopupLogo(false);
  //   WaitLogo();
  // }
  ApplyOptions();
  //printf("gManager = 0x%08x\n",gManager);   fflush(stdout);
  //gManager->Print();
  //gManager->Connect("TObjectManager", "ObjectAppended(TObject*)", "TGRUTint", this, "ObjectAppended(TObject*)");
}

void TGRUTint::SplashPopNWait(bool flag) {
  //PopupLogo(false);
  //WaitLogo();
}

void TGRUTint::LoadDetectorClasses() {
  TS800 s800;

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


  //TSega::LoadDetectorPositions();

  if(opt->StartGUI()){
    // TThread *gui_thread = new TThread("gui_thread",(TThread::VoidRtnFunc_t)GUI_Loop);
    // gui_thread->Run();

    std::string   script_filename = Form("%s/util/grut-view.py",getenv("GRUTSYS"));
    std::ifstream script(script_filename);
    std::string   script_text((std::istreambuf_iterator<char>(script)),
                              std::istreambuf_iterator<char>());
    TPython::Exec(script_text.c_str());

    TTimer* gui_timer = new TTimer("TPython::Exec(\"update()\");", 10, true);
    gui_timer->TurnOn();
  }

  //if I am passed any calibrations, lets load those.
  if(opt->CalInputFiles().size()) {
    for(int x=0;x<opt->CalInputFiles().size();x++) {
      TChannel::ReadCalFile(opt->CalInputFiles().at(x).c_str());
    }
  }
  TDataLoop *loop = 0;
  //next most important thing, if given a raw file && told NOT to not sort!
  if(opt->RawInputFiles().size() && opt->SortRaw()) {
    for(int x=0;x<opt->RawInputFiles().size();x++) {
      loop = TDataLoop::Get("1_input_loop",new TRawFileIn(opt->RawInputFiles().at(x).c_str()));
      loop->Resume();
      TBuildingLoop *boop = TBuildingLoop::Get("2_build_loop",loop);
      boop->Resume();
      TUnpackingLoop *uoop1 = TUnpackingLoop::Get("3_unpack1",boop);
      uoop1->Resume();
      //TUnpackingLoop *uoop2 = TUnpackingLoop::Get("unpack2",boop);
      //uoop2->Resume();
      // TUnpackingLoop *uoop3 = TUnpackingLoop::Get("unpack3",boop);
      // uoop3->Resume();

      std::string rootoutfile = "run" + get_run_number(opt->RawInputFiles().at(x)) + ".root";
      if(opt->OutputFile().length()) {
        rootoutfile = opt->OutputFile();
      }

      TWriteLoop* woop = TWriteLoop::Get("4_write_loop", rootoutfile);
      woop->Connect(uoop1);
      //woop->Connect(uoop2);

      std::string histoutfile = "hist" + get_run_number(opt->RawInputFiles().at(x)) + ".root";
      // if(opt->OutputFile().length()) {
      //   histoutfile = opt->OutputFile();
      // }
      if(TGRUTOptions::Get()->MakeHistos()){
        THistogramLoop *hoop = THistogramLoop::Get("5_hist_loop");
        hoop->SetOutputFilename(histoutfile);
        woop->AttachHistogramLoop(hoop);
        hoop->Resume();
      }
      woop->Resume();
    }
      //printf("I was passed rawfile %s\n",opt->RawInputFiles().at(x).c_str());
  }

  //ok now, if told not to sort open any raw files as _data# (rootish like??)
  if(opt->RawInputFiles().size() && !opt->SortRaw()) {
    for(int x=0;x<opt->RawInputFiles().size();x++)
      OpenRawFile(opt->RawInputFiles().at(x));
      //printf("I was passed rawfile %s\n",opt->RawInputFiles().at(x).c_str());
  }

  //next, if given a root file and NOT told to sort it..
  if(opt->RootInputFiles().size()){
    for(int x=0;x<opt->RootInputFiles().size();x++)
      OpenRootFile(opt->RootInputFiles().at(x));

    //now that my root file has been open, I may need to re-apply any passed in calfiles.
    for(int x=0;x<opt->CalInputFiles().size();x++)
      printf("I am reloading calfile %s!\n",opt->CalInputFiles().at(x).c_str());
  }


  //next, if given a root file and told to sort it.
  if(gChain && opt->SortTree()){
    printf("Attempting to sort root files.\n");
    TChainLoop *coop = TChainLoop::Get("",gChain);
    THistogramLoop *hoop = THistogramLoop::Get("");
    coop->AttachHistogramLoop(hoop);
    hoop->Resume();
    coop->Resume();
    //for(int x=0;x<opt->RootInputFiles().size();x++)
    //  printf("I am going to sort this rootfile %s, someday.\n",opt->RootInputFiles().at(x).c_str());
  }


  if(opt->ExitAfterSorting()){
    if(loop) {
      while(loop->IsRunning()) {
        std::cout << "\r" << loop->Status() << std::flush;
        gSystem->ProcessEvents();
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    }
    std::cout << std::endl;
    this->Terminate();
  }



  //TDetectorEnv::Get(opt->DetectorEnvironment().c_str());
/*
  bool need_loop = false;
  if(opt->RawInputFiles().size()==1 && opt->SortRaw()){
    std::string filename = opt->RawInputFiles()[0];
    std::string outfile = opt->OutputFile();
    if(!outfile.length() && !opt->IsOnline()){
      outfile = opt->GenerateOutputFilename(filename);
    }
    //TGRUTLoop::Get()->ProcessFile(filename.c_str(), outfile.c_str());
    need_loop = true;

  } else if (opt->RawInputFiles().size()>1 && opt->SortRaw()){
    std::vector<std::string> filenames = opt->RawInputFiles();
    std::string outfile = opt->OutputFile();
    if(!outfile.length() && !opt->IsOnline()){
      outfile = opt->GenerateOutputFilename(filenames);
    }
    //TGRUTLoop::Get()->ProcessFile(filenames, outfile.c_str());
    need_loop = true;
  }

  //if(!opt->StartGUI()) {
  //  for(auto& filename : opt->RootInputFiles()){
  //    OpenRootFile(filename);
  //  }
  //}

  for(auto& filename : opt->MacroInputFiles()){
    RunMacroFile(filename);
  }

  if(opt->SortTree() && !opt->StartGUI()) {
    opt->SetStartGUI();
  }

  if(opt->StartGUI()){
    //std::string script_filename = program_path() + "/../util/grut-view.py";
    std::string script_filename = Form("%s/../util/grut-view.py",getenv("GRSISYS"));
    std::ifstream script(script_filename);
    std::string script_text((std::istreambuf_iterator<char>(script)),
                            std::istreambuf_iterator<char>());
    TPython::Exec(script_text.c_str());

    for(auto& filename : opt->RootInputFiles()){
      TPython::Exec(Form("window.LoadRootFile(\"%s\")",filename.c_str()));
    }
    for(auto& filename : opt->GuiSaveSetFiles()){
      TPython::Exec(Form("window.LoadGuiFile(\"%s\")",filename.c_str()));
    }

    for(auto& filename : opt->RootInputFiles()){
      TPython::Exec(Form("window.LoadRootFile(\"%s\")",filename.c_str()));
      //OpenRootFile(filename); // Is this needed/sane?
    }
    //fGuiTimer = new TTimer("TPython::Exec(\"update()\");",100);
    //fGuiTimer->TurnOn();
  }

  //if(opt->SortTree() && fChain->GetEntries()){
  //  TGRUTLoop::Get()->ProcessTree(fChain, "/dev/null");
  //  need_loop = true;
  //}

  //if (need_loop){
  //  TGRUTLoop::Get()->Start();
  //}

  if(opt->ExitAfterSorting()){
    //TGRUTLoop::Get()->Status();
    //std::cout << "Waiting for join" << std::endl;
    //TGRUTLoop::Get()->Join();
    this->Terminate();
  } //else if(opt->CommandServer()) {
    //fCommandServer = new TGRUTServer(opt->CommandPort());
    //fCommandServer->Start();
    //fCommandTimer = new TTimer("",10);
    //fCommandTimer->TurnOn();
  //}
  //
  */
}

//void TGRUTint::DefaultFunction(){
//  static int i = 0;
//  std::cout << "I am a default function." << std::endl;
//  std::cout << "I have been called " << i++ << " times before" << std::endl;
//}


TFile* TGRUTint::OpenRootFile(const std::string& filename, Option_t* opt){
  TString sopt(opt);
  sopt.ToLower();

  bool is_online = sopt.Contains("online");
  sopt.ReplaceAll("online","");

  TFile* file = NULL;
  if(sopt.Contains("recreate") ||
     sopt.Contains("new")) {
    file = new TFile(filename.c_str(), "RECREATE");
    if(file){
      const char* command = Form("TFile* _file%i = (TFile*)%luL",
                                 fRootFilesOpened,
                                 (unsigned long)file);
      ProcessLine(command);
      fRootFilesOpened++;
    } else {
      std::cout << "Could not create " << filename << std::endl;
    }
  } else {
    file = new TFile(filename.c_str(), opt);
    if(file){
      const char* command = Form("TFile* _%s%i = (TFile*)%luL",
                                 is_online ? "online" : "file",
                                 fRootFilesOpened,
                                 (unsigned long)file);
      ProcessLine(command);
      std::cout << "\tfile " << BLUE << file->GetName() << RESET_COLOR
                <<  " opened as " << BLUE <<  "_file" << fRootFilesOpened << RESET_COLOR <<  std::endl;

      if(file->FindObjectAny("EventTree")) {
        if(!gChain)
          gChain = new TChain("EventTree");
        gChain->Add(file->GetName());
      }
      fRootFilesOpened++;
    } else {
      std::cout << "Could not open " << filename << std::endl;
    }
  }


  if(is_online){
    file->SetOption("online");
  }

  if(file && TGRUTOptions::Get()->StartGUI()){
    TPython::Bind(file,"tdir");
    ProcessLine("TPython::Exec(\"window.AddDirectory(tdir)\");");
  }
  return file;
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
//  if(!file_exists(filename)){
//    std::cerr << "File \"" << filename << "\" does not exist" << std::endl;
//    return;
//  }

//  const char* command = Form(".x %s", filename.c_str());
//  ProcessLine(command);
}

/*
void TGRUTint::HandleFile(const std::string& filename){
  if(!file_exists(filename)){
    std::cerr << "File \"" << filename << "\" does not exist" << std::endl;
    return;
  }

  TGRUTOptions* opt = TGRUTOptions::Get();

  kFileType filetype = opt->DetermineFileType(filename);
  switch(filetype){
  case NSCL_EVT:
  case GRETINA_MODE2:
  case GRETINA_MODE3:

    std::string outfile = opt->OutputFile();
    if(!outfile.length()){
      outfile = opt->GenerateOutputFilename(filename);
    }
    TGRUTLoop::Get()->ProcessFile(filename.c_str(), outfile.c_str());
  }
  break;

  case ROOT_DATA:
    OpenRootFile(filename);
    break;

  case ROOT_MACRO:
    RunMacroFile(filename);
    break;

  default:
    std::cerr << "Unknown file type for " << filename << std::endl;
  }
}
*/


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


//TObject* TGRUTint::ObjectAppended(TObject* obj){
  //fNewChild = obj;
//}

/*
TString TGRUTint::ReverseObjectSearch(TString &input) {

  int end = 0;
  int start = input.Length();
  TString output;
  for (int i = start; i > 0; i--) {
    if (input[i] == '.') {
      return TString(input(0,i));
    }
    if (input[i] == '>' && i > 0 && input[i-1] == '-') {
      return TString(input(0,i-1));
    }
  }
  return TString("");
};
*/

void TGRUTint::Terminate(Int_t status){
  StoppableThread::StopAllClean();

  //if(fCommandServer) {
  //  fCommandServer->Stop();
  //  fCommandServer->Delete();
  //  fCommandServer = NULL;
  //}

  //if(TGRUTOptions::Get()->StartGUI()){
  //  TPython::Exec("on_close()");
  //}

  //Be polite when you leave.
  printf(DMAGENTA "\nbye,bye\t" DCYAN "%s" RESET_COLOR  "\n",getlogin());

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

/*
void TGRUTint::OpenFileDialog() {
  TGFileInfo file_info;
  const char *filetypes[] = { "ROOT File", "*.root",
                              "Macro File", "*.C",
                              "GRETINA data file","*.dat",
                              "NSCL data","*.evt",
                              "Calibrtaion file","*.cal",
                              0,0 };
  file_info.fFileTypes = filetypes;
  new TGFileDialog(gClient->GetRoot(),0,kFDOpen,&file_info);
  if(file_info.fFilename)  {
    HandleFile(file_info.fFilename);
  }
}
*/


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
