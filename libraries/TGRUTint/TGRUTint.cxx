#include <Globals.h>
#include "TGRUTint.h"

#include "TFile.h"

#include "TDetectorEnv.h"
#include "TGRUTOptions.h"
#include "TGRUTLoop.h"

#include <TROOT.h>


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

  ApplyOptions();
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
    std::string outfile = TGRUTOptions::Get()->OutputFile();
    if(!outfile.length()){
      outfile = TGRUTOptions::Get()->GenerateOutputFilename(filename);
    }
    TGRUTLoop::Get()->ProcessFile(filename.c_str(), outfile.c_str());
    TGRUTLoop::Get()->Start();
  }

  for(unsigned int i=0; i<TGRUTOptions::Get()->RootInputFiles().size(); i++) {
    const char* command = Form("TFile *_file%i = new TFile(\"%s\",\"read\");", i, TGRUTOptions::Get()->RootInputFiles().at(i).c_str());
    ProcessLine(command);

    TFile *file = (TFile*)gROOT->FindObject(TGRUTOptions::Get()->RootInputFiles().at(i).c_str());
    std::cout << "\tfile " << file->GetName() << " opened as _file" << i << std::endl;

  }

  if(TGRUTOptions::Get()->ExitAfterSorting()){
    TGRUTLoop::Get()->Status();
    TGRUTLoop::Get()->Join();
    gApplication->Terminate();
  }
}
