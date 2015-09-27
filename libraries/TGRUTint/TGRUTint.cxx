#include <Globals.h>
#include "TGRUTint.h"

#include <fstream>

#include "TFile.h"

#include "TInterpreter.h"
#include "TDetectorEnv.h"
#include "TGRUTOptions.h"
#include "TGRUTLoop.h"
#include "TObjectManager.h"
#include "TGRUTUtilities.h"
#include "GRootGuiFactory.h"
#include "TUnixSystem.h"
//#include "Api.h"   // for G__value

#include <Getline.h>
#include <TClass.h>
#include <TROOT.h>
#include <TString.h>

#include <TTree.h>
#include <TH1.h>
#include <TMode3.h>

#include <TGFileDialog.h>

//extern "C" G__value G__getitem(const char* item);
//#include "FastAllocString.h"
//char* G__valuemonitor(G__value buf, G__FastAllocString& temp);

extern void PopupLogo(bool);
extern void WaitLogo();

ClassImp(TGRUTint)

TGRUTint *TGRUTint::fTGRUTint = NULL;
TEnv    *TGRUTint::fGRUTEnv  = NULL;
TObject *gResponse = NULL;


TGRUTint *TGRUTint::instance(int argc,char** argv, void *options, int numOptions, bool noLogo, const char *appClassName) {
  if(!fTGRUTint) {
    fTGRUTint = new TGRUTint(argc,argv,options,numOptions,true,appClassName);
    fTGRUTint->Init();
  }
  return fTGRUTint;
}


TGRUTint::TGRUTint(int argc, char **argv,void *options, Int_t numOptions, Bool_t noLogo,const char *appClassName)
  :TRint(appClassName, &argc, argv, options, numOptions,noLogo),
   fCommandServer(NULL), fCommandTimer(NULL), fRootFilesOpened(0), fIsTabComplete(false) {

  fGRUTEnv = gEnv;
  GetSignalHandler()->Remove();
  TGRUTInterruptHandler *ih = new TGRUTInterruptHandler();
  ih->Add();
  TH1::SetDefaultSumw2();

  SetPrompt("GRizer [%d] ");

  auto opt = TGRUTOptions::Get(argc, argv);
}


TGRUTint::~TGRUTint() {
  if(fCommandTimer){
    delete fCommandTimer;
  }
}


void TGRUTint::Init() {
  TGRUTLoop::CreateDataLoop<TGRUTLoop>();

  if(TGRUTOptions::Get()->ShouldExit()){
    gApplication->Terminate();
    return;
  }
  //printf("%s called.\n",__PRETTY_FUNCTION__);
//  TMode3 *mode3 = new TMode3;
//  mode3->Class();

  TFile *tempfile = TFile::Open("/var/tmp/mytemp.root","recreate");
  TTree *temptree = new TTree("temp","temp");
  TMode3 *mode3=0;
  temptree->Branch("TMode3","TMode3",&mode3);
  temptree->Fill();
  temptree->Write();
  delete tempfile;
  gSystem->Unlink("/var/tmp/mytemp.root");


  if(TGRUTOptions::Get()->MakeBackupFile()){
    TObjectManager::Get("GRUT_Manager", "GRUT Manager");
  }



  std::string grutpath = getenv("GRUTSYS");

  gInterpreter->AddIncludePath(Form("%s/include",grutpath.c_str()));
  //gSystem->AddIncludePath(Form("-I%s/include",grutpath.c_str()));
  //gSystem->AddDynamicPath(Form("-%s/libraries",grutpath.c_str()));

  if(TGRUTOptions::Get()->ShowLogo()){
    PopupLogo(false);
    WaitLogo();
  }

  // if(TGRUTOptions::Get()->ShowLogo()){
  //   PopupLogo(false);
  //   WaitLogo();
  // }
  ApplyOptions();
  //printf("gManager = 0x%08x\n",gManager);   fflush(stdout);
  //gManager->Print();
  gManager->Connect("TObjectManager", "ObjectAppended(TObject*)", "TGRUTint", this, "ObjectAppended(TObject*)");
}

/*********************************/

bool TGRUTInterruptHandler::Notify() {
  static int timespressed  = 0;
  timespressed++;
  if(timespressed>3) {
    printf("\n" DRED BG_WHITE  "   Quit hitting me... Force exiting. :( " RESET_COLOR "\n");
    exit(1);
  }
  printf("\n" DRED BG_WHITE  "   Control-c was pressed.   " RESET_COLOR "\n");
  TGRUTint::instance()->Terminate();
  return true;
}

void TGRUTint::ApplyOptions() {
  TGRUTOptions* opt = TGRUTOptions::Get();

  if(!false) { //this can be change to something like, if(!ClassicRoot)
     LoadGRootGraphics();
  }

  TDetectorEnv::Get(opt->DetectorEnvironment().c_str());

  if(opt->RawInputFiles().size()==1 && opt->SortRaw()){
    std::string filename = opt->RawInputFiles()[0];
    std::string outfile = opt->OutputFile();
    if(!outfile.length()){
      outfile = opt->GenerateOutputFilename(filename);
    }
    TGRUTLoop::Get()->ProcessFile(filename.c_str(), outfile.c_str());
    TGRUTLoop::Get()->Start();

  } else if (opt->RawInputFiles().size()>1 && opt->SortRaw()){
    std::vector<std::string> filenames = opt->RawInputFiles();
    std::string outfile = opt->OutputFile();
    if(!outfile.length()){
      outfile = opt->GenerateOutputFilename(filenames);
    }
    TGRUTLoop::Get()->ProcessFile(filenames, outfile.c_str());
    TGRUTLoop::Get()->Start();
  }

  for(auto& filename : opt->RootInputFiles()){
    OpenRootFile(filename);
  }

  for(auto& filename : opt->MacroInputFiles()){
    RunMacroFile(filename);
  }

  if(TGRUTOptions::Get()->ExitAfterSorting()){
    TGRUTLoop::Get()->Status();
    std::cout << "Waiting for join" << std::endl;
    TGRUTLoop::Get()->Join();
    this->Terminate();
  } else if(TGRUTOptions::Get()->CommandServer()) {
    fCommandServer = new TGRUTServer(TGRUTOptions::Get()->CommandPort());
    fCommandServer->Start();
    fCommandTimer = new TTimer("",10);
    fCommandTimer->TurnOn();
  }
}

void TGRUTint::DefaultFunction(){
  static int i = 0;
  std::cout << "I am a default function." << std::endl;
  std::cout << "I have been called " << i++ << " times before" << std::endl;
}

void TGRUTint::OpenRootFile(const std::string& filename){
  if(!file_exists(filename)){
    std::cerr << "File \"" << filename << "\" does not exist" << std::endl;
    return;
  }

  TGRUTOptions* opt = TGRUTOptions::Get();

  const char* command = Form("TFile *_file%i = TObjectManager::Get(\"%s\",\"read\")",
			      fRootFilesOpened, filename.c_str());
  //const char* command = Form("TFile *_file%i = new TFile(\"%s\",\"read\")",
  ProcessLine(command);

  TFile *file = (TFile*)gROOT->GetListOfFiles()->FindObject(filename.c_str());
  if(file){
    std::cout << "\tfile " << file->GetName() << " opened as _file" << fRootFilesOpened << std::endl;
  }

  fRootFilesOpened++;
}

void TGRUTint::RunMacroFile(const std::string& filename){
  if(!file_exists(filename)){
    std::cerr << "File \"" << filename << "\" does not exist" << std::endl;
    return;
  }

  const char* command = Form(".x %s", filename.c_str());
  ProcessLine(command);
}

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
  {
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
  sline.ReplaceAll("TCanvas","GCanvas");

  if(sline.Contains("for") ||
     sline.Contains("while") ||
     sline.Contains("if") ||
     sline.Contains("{")){
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


  fNewChild = NULL;
  long result =  TRint::ProcessLine(sline.Data(),sync,error);

  if(!fNewChild){
    return result;
  }

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
  return result;
}


TObject* TGRUTint::ObjectAppended(TObject* obj){
  fNewChild = obj;
}


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

void TGRUTint::Terminate(Int_t status){
  if(fCommandServer) {
    fCommandServer->Stop();
    fCommandServer->Delete();
    fCommandServer = NULL;
  }
  TGRUTLoop::Get()->Stop();

  TIter iter(TObjectManager::GetListOfManagers());
  while(TObjectManager *om = (TObjectManager*)iter.Next()) {
    om->SaveAndClose();
  }

  TRint::Terminate(status);
}

void TGRUTint::LoadGRootGraphics() {
  if(gROOT->IsBatch()) return;
  gROOT->LoadClass("TCanvas","Gpad");
  gGuiFactory = new GRootGuiFactory();

}


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
  return;
}

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
