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

//#include "GRootGuiFactory.h"
//#include "ProgramPath.h"
//#include "TDetectorEnv.h"
#include "TGRUTOptions.h"
#include "TRawSource.h"
//#include "TGRUTLoop.h"
#include "TGRUTUtilities.h"
//#include "TObjectManager.h"

//#include "TOnlineTree.h"

//extern "C" G__value G__getitem(const char* item);
//#include "FastAllocString.h"
//char* G__valuemonitor(G__value buf, G__FastAllocString& temp);

//extern void PopupLogo(bool);
//extern void WaitLogo();

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
   fIsTabComplete(false) {
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
  //TGRUTLoop::CreateDataLoop<TGRUTLoop>();
  //fChain = new TChain("EventTree");

  if(TGRUTOptions::Get()->ShouldExit()){
    Terminate();
    return;
  }
  //printf("%s called.\n",__PRETTY_FUNCTION__);
//  TMode3 *mode3 = new TMode3;
//  mode3->Class();

  // TFile *tempfile = TFile::Open("/var/tmp/mytemp.root","recreate");
  // TTree *temptree = new TTree("temp","temp");
  // TMode3 *mode3=0;
  // temptree->Branch("TMode3","TMode3",&mode3);
  // temptree->Fill();
  // temptree->Write();
  // delete tempfile;
  // gSystem->Unlink("/var/tmp/mytemp.root");


  if(TGRUTOptions::Get()->MakeBackupFile()){
    //TObjectManager::Get("GRUT_Manager", "GRUT Manager");
  }



  std::string grutpath = getenv("GRUTSYS");
  gInterpreter->AddIncludePath(Form("%s/include",grutpath.c_str()));
  //gSystem->AddIncludePath(Form("-I%s/include",grutpath.c_str()));
  //gSystem->AddDynamicPath(Form("-%s/libraries",grutpath.c_str()));

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

void TGRUTint::ApplyOptions() {
  TGRUTOptions* opt = TGRUTOptions::Get();

  if(!false) { //this can be change to something like, if(!ClassicRoot)
     //LoadGRootGraphics();
  }

  //if I am passed any calibrations, lets load those.
  if(opt->CalInputFiles().size()) {
    for(int x=0;x<opt->CalInputFiles().size();x++)
      printf("I was passed calfile %s\n",opt->CalInputFiles().at(x).c_str());
  }

  //next most important thing, if given a raw file && told NOT to not sort!
  if(opt->RawInputFiles().size() && opt->SortRaw()) {
    for(int x=0;x<opt->RawInputFiles().size();x++) 
      printf("I was passed rawfile %s\n",opt->RawInputFiles().at(x).c_str());
  }  

  //ok now, if told not to sort open any raw files as _data# (rootish like??)
  if(opt->RawInputFiles().size() && !opt->SortRaw()) {
    for(int x=0;x<opt->RawInputFiles().size();x++) 
      OpenRawFile(opt->RawInputFiles().at(x));
      //printf("I was passed rawfile %s\n",opt->RawInputFiles().at(x).c_str());
  }

  //next, if given a root file and told to sort it.
  if(opt->RootInputFiles().size() && opt->SortTree()){
    for(int x=0;x<opt->RootInputFiles().size();x++) 
      printf("I am going to sort this rootfile %s, someday.\n",opt->RootInputFiles().at(x).c_str());
  }  

  //next, if given a root file and NOT told to sort it..
  if(opt->RootInputFiles().size() && !opt->SortTree()){
    for(int x=0;x<opt->RootInputFiles().size();x++) 
      OpenRootFile(opt->RawInputFiles().at(x));
    //now that my root file has been open, I may need to re-apply any passed in calfiles.
    for(int x=0;x<opt->CalInputFiles().size();x++)
      printf("I am reloading calfile %s!\n",opt->CalInputFiles().at(x).c_str());
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


TFile* TGRUTint::OpenRootFile(const std::string& filename, TChain *chain){
  if(!file_exists(filename.c_str())){
    std::cerr << "File \"" << filename << "\" does not exist" << std::endl;
    return NULL;
  }

  // const char* command = Form("TFile *_file%i = TObjectManager::Get(\"%s\",\"read\")",
  //       		      fRootFilesOpened, filename.c_str());
  const char* command = Form("TFile *_file%i = new TFile(\"%s\",\"read\")",
                             fRootFilesOpened, filename.c_str());
  ProcessLine(command);
  TFile *file = (TFile*)gROOT->GetListOfFiles()->FindObject(filename.c_str());
  if(file){
    std::cout << "\tfile " << file->GetName() << " opened as _file" << fRootFilesOpened << std::endl;
    if(file->FindObjectAny("EventTree")) {
      if(!fChain)
        fChain = new TChain("EventTree");
      fChain->Add(file->GetName());
    }
  }
  fRootFilesOpened++;
  return file;
}

TRawFileIn *TGRUTint::OpenRawFile(const std::string& filename) {
  if(!file_exists(filename.c_str())){
    std::cerr << "File \"" << filename << "\" does not exist" << std::endl;
    return NULL;
  }
  const char* command1 = Form("TGRUTInt::Get()->GetListOfRawFiles()->Add(new TRawFileIn(\"%s\")",filename.c_str());
  ProcessLine(command1);
  const char* command2 = Form("TRawFileIn *_data%i = (TRawFileIn*)TGRUTInt::Get()->GetListOfRawFiles()->Last()",fRawFilesOpened);
  ProcessLine(command2);
  TRawFileIn *file = (TRawFileIn*)GetListOfRawFiles()->Last();
  if(file){
    //std::string name = file->GetName();
    //std::replace(name.begin(),name.end(),'_','/');
    std::cout << "\tfile " << filename << " opened as _data" << fRawFilesOpened << std::endl;
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
  if(fIsTabComplete){
    return TRint::ProcessLine(line, sync, error);
  }
  TString sline(line);
  if(!sline.Length()){
    return 0;
  }
  //sline.ReplaceAll("TCanvas","GCanvas");

  if(sline.Contains("for") ||
     sline.Contains("while") ||
     sline.Contains("if") ||
     sline.Contains("{") ||
     sline.Contains("TPython")){
     return TRint::ProcessLine(sline.Data(), sync, error);
  }

  if(!sline.CompareTo("clear")) {
    return TRint::ProcessLine(".! clear");
  }

  Ssiz_t index;

  if((index = sline.Index(';')) != kNPOS){
    TString first = sline(0,index);
    first = first.Strip(TString::kTrailing);
    long res = ProcessLine(first.Data(),     sync, error);
    if(sline.Length() > index){
      TString second = sline(index+1, sline.Length()-index).Data();
      second = second.Strip(TString::kLeading);
      res += ProcessLine(second.Data(), sync, error);
    }
    return res;
  }


  //fNewChild = NULL;
  long result =  TRint::ProcessLine(sline.Data(),sync,error);

  //if(!fNewChild){
  //  return result;
  //}
/*
  if((index = sline.Index("Project",TString::kIgnoreCase))    != kNPOS   ||
     (index = sline.Index("Projection",TString::kIgnoreCase)) != kNPOS   ||
     (index = sline.Index("Profile",TString::kIgnoreCase))    != kNPOS   ||
     (index = sline.Index("Draw",TString::kIgnoreCase))       != kNPOS ) {

    TString newline = ReverseObjectSearch(sline);
    TObject *parent = gROOT->FindObject(newline.Data());

    if(parent){
      gManager->AddRelationship(parent, fNewChild);
    }
  }

  fNewChild = NULL;
*/
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
  //if(fCommandServer) {
  //  fCommandServer->Stop();
  //  fCommandServer->Delete();
  //  fCommandServer = NULL;
  //}

  //if(TGRUTOptions::Get()->StartGUI()){
  //  TPython::Exec("on_close()");
  //}

  //TGRUTLoop::Get()->Stop();
  //TDataLoop::DeleteInstance();

  // TIter iter(TObjectManager::GetListOfManagers());
  // while(TObjectManager *om = (TObjectManager*)iter.Next()) {
  //   om->SaveAndClose();
  // }

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
  //GRootGuiFactory::Init();
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
/*
TObject* TGRUTint::DelayedProcessLine(std::string message){
  std::lock_guard<std::mutex> any_command_lock(fCommandWaitingMutex);

  {
    std::lock_guard<std::mutex> lock(fCommandListMutex);
    fLinesToProcess.push(message);
  }

  TTimer::SingleShot(0,"TGRUTint",this,"DelayedProcessLine_ProcessItem()");

  std::unique_lock<std::mutex> lock(fResultListMutex);
  while(fCommandResults.empty()){
    fNewResult.wait(lock);
  }

  TObject* result = fCommandResults.front();
  fCommandResults.pop();
  return result;
}
*/
/*
void TGRUTint::DelayedProcessLine_ProcessItem(){
  std::string message;
  {
    std::lock_guard<std::mutex> lock(fCommandListMutex);
    if(fLinesToProcess.empty()){
      return;
    }
    message = fLinesToProcess.front();
    fLinesToProcess.pop();
  }

  std::cout << "Received remote command \"" << message << "\"" << std::endl;
  this->ProcessLine(message.c_str());
  Getlinem(EGetLineMode::kInit,((TRint*)gApplication)->GetPrompt());

  {
    std::lock_guard<std::mutex> lock(fResultListMutex);
    fCommandResults.push(gResponse);
    gResponse = NULL;
  }

  fNewResult.notify_one();
}
*/
