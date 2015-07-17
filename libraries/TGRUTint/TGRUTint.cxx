

#include <Globals.h>

#include <TGRUTint.h>

#include <TROOT.h>

#include <Globals.h>

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

   SetPrompt("GRizer [%d] ");

   Init();

}


TGRUTint::~TGRUTint() {   }


void TGRUTint::Init() {
   //std::string geb_path = getenv("GRUTSYS");
   //gROOT->ProcessLine(Form("#include \"%s/include/TGRUTStructs.h\"",geb_path.c_str()));

   PopupLogo(false);
   WaitLogo();

}

/*********************************/

bool TGRUTInterruptHandler::Notify() {
   printf("\n" DRED BG_WHITE  "   Control-c was pressed.   " RESET_COLOR "\n");
   gApplication->Terminate();
   return true;
}




