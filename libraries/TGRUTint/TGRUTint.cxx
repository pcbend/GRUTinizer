#include <Globals.h>
#include "TGRUTint.h"

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
    printf("\n" DRED BG_WHITE  "   Force exiting.   " RESET_COLOR "\n");
    exit(1);
  }
  printf("\n" DRED BG_WHITE  "   Control-c was pressed.   " RESET_COLOR "\n");
  gApplication->Terminate();
  return true;
}

void TGRUTint::ApplyOptions() {
  TGRUTOptions* opt = TGRUTOptions::Get();


  if(opt->RawInputFiles().size() && opt->SortRaw()){
    for(auto filename : opt->RawInputFiles()){
      TDataLoop::Instance()->ProcessFile(filename.c_str());
    }
  }

  TDetectorEnv::Get(opt->DetectorEnvironment().c_str());
}
