
#include <Globals.h>

#include <cstdio>
#include <string>
//#include <sys/stat.h>
//#include <netdb.h>

#include "TEnv.h"
#include "TPluginManager.h"

#include "TGRUTint.h"
#include "ProgramPath.h"

#ifdef __APPLE__
#define HAVE_UTMPX_H
#define UTMP_NO_ADDR
#ifndef ut_user
#   define ut_user ut_name
#endif
#ifndef UTMP_FILE
#define UTMP_FILE "/etc/utmp"
#endif
#endif


# ifdef HAVE_UTMPX_H
# include <utmpx.h>
# define STRUCT_UTMP struct utmpx
# else
# if defined(__linux) && defined(__powerpc) && (__GNUC__ == 2) && (__GNUC_MINOR__ < 90)
   extern "C" {
# endif
# include <utmp.h>
# define STRUCT_UTMP struct utmp
# endif




void SetGRUTEnv();
void SetGRUTPluginHandlers();
//static int ReadUtmp();
//static STRUCT_UTMP *SearchEntry(int, const char*);
//static void SetDisplay();



int main(int argc, char **argv) {
   //Find the grsisort environment variable so that we can read in .grsirc
   //SetDisplay();
   SetGRUTEnv();
   //SetGRUTPluginHandlers();
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
  // Set the GRUTSYS variable based on the executable path.
  // If GRUTSYS has already been defined, don't overwrite.
  setenv("GRUTSYS", (program_path()+"/..").c_str(), 0);

  std::string grut_path = program_path() + "/../.grutrc";
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
