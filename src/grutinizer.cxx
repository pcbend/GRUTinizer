
#include <Globals.h>

#include <cstdio>
#include <string>
//#include <sys/stat.h>
//#include <netdb.h>

#include "TEnv.h"
#include "TPluginManager.h"
#include "TGRUTint.h"


void SetGRUTEnv();
void SetGRUTPluginHandlers();
//static int ReadUtmp();
//static STRUCT_UTMP *SearchEntry(int, const char*);
//static void SetDisplay();



int main(int argc, char **argv) {
   //Find the grsisort environment variable so that we can read in .grsirc
   //SetDisplay();
//   SetGRUTEnv();
//   SetGRUTPluginHandlers();
   TGRUTint *input = 0;

   //Create an instance of the grsi interpreter so that we can run root-like interpretive mode
   input = TGRUTint::instance(argc,argv);
   //input->GetOptions(&argc,argv);
   //Run the code!
   input->Run("true");
   //Be polite when you leave.
   printf("\nbye,bye\n");

   return 0;
}


void SetGRUTEnv() {
   std::string grut_path = getenv("GRUTSYS"); //Finds the GRUTSYS path to be used by other parts of the grsisort code
   if(grut_path.length()>0) {
      grut_path += "/";
   }
   //Read in grsirc in the GRSISYS directory to set user defined options on grsisort startup
   grut_path +=  ".grutrc";
   gEnv->ReadFile(grut_path.c_str(),kEnvChange);
}

void SetGRUTPluginHandlers() {
   //gPluginMgr->AddHandler("GRootCanvas","grsi","GRootCanvas"
//   gPluginMgr->AddHandler("TGuiFactory","root","GROOTGuiFactory","Gui","GROOTGuiFactory()");
//   gPluginMgr->AddHandler("TBrowserImp","GRootBrowser","GRootBrowser",
//                          "Gui","NewBrowser(TBrowser *,const char *,Int_t,Int_t,UInt_t,UInt_t");
//   gPluginMgr->AddHandler("TBrowserImp","GRootBrowser","GRootBrowser",
//                          "Gui","NewBrowser(TBrowser *,const char *,Int_t,Int_t");
}
