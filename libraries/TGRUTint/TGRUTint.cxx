#include <Globals.h>
#include "TGRUTint.h"

#include <fstream>

#include "TFile.h"

#include "TDetectorEnv.h"
#include "TGRUTOptions.h"
#include "TGRUTLoop.h"
#include "TObjectManager.h"

//#include "Api.h"   // for G__value

#include <TClass.h>
#include <TROOT.h>
#include <TString.h>


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
  :TRint(appClassName, &argc, argv, options, numOptions,noLogo) {

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


TGRUTint::~TGRUTint() {   }


void TGRUTint::Init() {

  // if(TGRUTOptions::Get()->ShowLogo()){
  //   PopupLogo(false);
  //   WaitLogo();
  // }
  TGRUTLoop::CreateDataLoop<TGRUTLoop>();
  TObjectManager::Init("GRUT_Manager", "GRUT Manager");
  ApplyOptions();
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

  for(unsigned int i=0; i<TGRUTOptions::Get()->RootInputFiles().size(); i++) {
    const char* command = Form("TFile *_file%i = new TFile(\"%s\",\"read\");", i, TGRUTOptions::Get()->RootInputFiles().at(i).c_str());
    ProcessLine(command);

    TFile *file = (TFile*)gROOT->FindObjectAny(TGRUTOptions::Get()->RootInputFiles().at(i).c_str());
    std::cout << "\tfile " << file->GetName() << " opened as _file" << i << std::endl;

  }

  if(TGRUTOptions::Get()->ExitAfterSorting()){
    TGRUTLoop::Get()->Status();
    TGRUTLoop::Get()->Join();
    gApplication->Terminate();
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
