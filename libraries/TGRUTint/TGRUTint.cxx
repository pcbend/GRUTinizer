#include <Globals.h>
#include "TGRUTint.h"

#include <fstream>

#include "TFile.h"

#include "TDetectorEnv.h"
#include "TGRUTOptions.h"
#include "TGRUTLoop.h"
#include "TObjectManager.h"
#include "TGRUTUtilities.h"

//#include "Api.h"   // for G__value

#include <Getline.h>
#include <TClass.h>
#include <TROOT.h>
#include <TString.h>

#include <TTree.h>
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


TGRUTint *TGRUTint::instance(int argc,char** argv, void *options, int numOptions, bool noLogo, const char *appClassName) {
  if(!fTGRUTint)
    fTGRUTint = new TGRUTint(argc,argv,options,numOptions,true,appClassName);
  return fTGRUTint;
}


TGRUTint::TGRUTint(int argc, char **argv,void *options, Int_t numOptions, Bool_t noLogo,const char *appClassName)
  :TRint(appClassName, &argc, argv, options, numOptions,noLogo), 
   fCommandServer(NULL), fCommandTimer(NULL), fRootFilesOpened(0) {

  fGRUTEnv = gEnv;
  GetSignalHandler()->Remove();
  TGRUTInterruptHandler *ih = new TGRUTInterruptHandler();
  ih->Add();

  SetPrompt("GRizer [%d] ");

  auto opt = TGRUTOptions::Get(argc, argv);
  if(opt->ShouldExit()){
    gApplication->Terminate();
    return;
  }

  Init();
}


TGRUTint::~TGRUTint() {   
  if(fCommandTimer){
    delete fCommandTimer;
  }
}


void TGRUTint::Init() {
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

//  TClass::LoadClass("TMode3",false);


  // if(TGRUTOptions::Get()->ShowLogo()){
  //   PopupLogo(false);
  //   WaitLogo();
  // }
  TGRUTLoop::CreateDataLoop<TGRUTLoop>();
  TObjectManager::Init("GRUT_Manager", "GRUT Manager");
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
  gApplication->Terminate();
  return true;
}

void TGRUTint::ApplyOptions() {
  TGRUTOptions* opt = TGRUTOptions::Get();

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
    gApplication->Terminate();
  } else if(TGRUTOptions::Get()->CommandServer()) {
    fCommandServer = new TGRUTServer(TGRUTOptions::Get()->CommandPort());
    fCommandServer->Start();
    fCommandTimer = new TTimer("TGRUTint::instance()->DelayedProcessLine_ProcessItem();", 100);
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

  const char* command = Form("TFile *_file%i = new TFile(\"%s\",\"read\")", 
			     fRootFilesOpened, filename.c_str());
  TRint::ProcessLine(command);

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
  TRint::ProcessLine(command);
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

  if(sline.Contains("for") ||
     sline.Contains("while") ||
     sline.Contains("if") ||
     sline.Contains("{")){
    return TRint::ProcessLine(line, sync, error);
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


  long result =  TRint::ProcessLine(line,sync,error);

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
  }
  TGRUTLoop::Get()->Stop();
  TRint::Terminate(status);
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

void TGRUTint::DelayedProcessLine(std::string message){
  std::lock_guard<std::mutex> lock(fCommandsMutex);
  fLinesToProcess.push(message);
}

void TGRUTint::DelayedProcessLine_ProcessItem(){
  std::string message;
  {
    std::lock_guard<std::mutex> lock(fCommandsMutex);
    if(fLinesToProcess.empty()){
      return;
    }
    message = fLinesToProcess.front();
    fLinesToProcess.pop();
  }

  std::cout << "Received remote command \"" << message << "\"" << std::endl;
  this->ProcessLine(message.c_str());
  Getlinem(EGetLineMode::kInit,((TRint*)gApplication)->GetPrompt());
}
