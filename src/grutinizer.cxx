
#include "Globals.h"

#include <cstdio>
#include <ctime>
#include <string>
//#include <sys/stat.h>
//#include <sys/socket.h>
//#include <netdb.h>

#include "TEnv.h"
#include "TPluginManager.h"
#include "TPython.h"
#include "TThread.h"

#include "TGRUTint.h"
#include "TGRUTUtilities.h"
#include "ProgramPath.h"
//#include "TROOT_Shim.h"
//#include "LoadGRUTEnv.h"



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

/*
#ifdef HAVE_UTMPX_H
#include <utmpx.h>
#define STRUCT_UTMP struct utmpx
#else
#include <utmp.h>
#define STRUCT_UTMP struct utmp
#endif
*/

//extern void PopupGrutLogo(bool);


//static int ReadUtmp();
//static STRUCT_UTMP *SearchEntry(int, const char*);
//static STRUCT_UTMP *gUtmpContents;

//static void SetDisplay();
void SetGRUTPluginHandlers();


void LoadGRUTEnv() {
  // Set the GRUTSYS variable based on the executable path.
  // If GRUTSYS has already been defined, don't overwrite.
  setenv("GRUTSYS", (program_path()+"/..").c_str(), 0);

  // Load $GRUTSYS/.grutrc
  std::string grut_path = Form("%s/.grutrc",getenv("GRUTSYS"));
  gEnv->ReadFile(grut_path.c_str(),kEnvChange);

  // Load $HOME/.grutrc
  grut_path = Form("%s/.grutrc",getenv("HOME"));
  if(file_exists(grut_path.c_str())){
    gEnv->ReadFile(grut_path.c_str(),kEnvChange);
  }
}

void AclicUseCpp11() {
  std::string cmd = gSystem->GetMakeSharedLib();
  cmd = ReplaceAll(cmd,"g++","g++ -std=c++11");
  gSystem->SetMakeSharedLib(cmd.c_str());
}

int main(int argc, char **argv) {
  //Find the grut environment variable so that we can read in .grutrc
  LoadGRUTEnv();
  SetGRUTPluginHandlers();
  AclicUseCpp11();
  //ReplaceCleanups();

  // This turns on both the Cint Mutex and gROOT Mutex,
  // with-out it, you are taking thread into your own hands
  // with-it, you can use TThread::Lock/UnLock around possible hazards.
  TThread::Initialize();

  //SetDisplay();
  TGRUTint *input = 0;

  //Create an instance of the grut interpreter so that we can run root-like interpretive mode
  input = TGRUTint::instance(argc,argv);
  //PopupGrutLogo(true);

  //Run the code!
  //input->Run(true);
  input->Run(false);
  return 0;
}



void SetGRUTPluginHandlers() {
  //gPluginMgr->AddHandler("GRootCanvas","grut","GRootCanvas"
  gPluginMgr->AddHandler("TGuiFactory","root","GROOTGuiFactory","Gui","GROOTGuiFactory()");
  gPluginMgr->AddHandler("TBrowserImp","GRootBrowser","GRootBrowser",
                         "Gui","NewBrowser(TBrowser *,const char *,Int_t,Int_t,UInt_t,UInt_t");
  gPluginMgr->AddHandler("TBrowserImp","GRootBrowser","GRootBrowser",
                         "Gui","NewBrowser(TBrowser *,const char *,Int_t,Int_t");
}

/*

static void SetDisplay()  {
  // Set DISPLAY environment variable.
  if(!getenv("DISPLAY")) {
    printf("Display not found, trying to set... \n");
    char *tty = ttyname(0);  // device user is logged in on
    if(tty) {
      tty += 5;             // remove "/dev/"
      STRUCT_UTMP *utmp_entry = SearchEntry(ReadUtmp(), tty);
      if(utmp_entry) {
        char *display = new char[sizeof(utmp_entry->ut_host) + 15];
        char *host = new char[sizeof(utmp_entry->ut_host) + 1];
        strncpy(host, utmp_entry->ut_host, sizeof(utmp_entry->ut_host));
        host[sizeof(utmp_entry->ut_host)] = 0;
        if(host[0]) {
          if(strchr(host, ':')) {
            sprintf(display, "DISPLAY=%s", host);
            fprintf(stderr, "*** DISPLAY not set, setting it to %s\n",
                    host);
          } else {
            sprintf(display, "DISPLAY=%s:0.0", host);
            fprintf(stderr, "*** DISPLAY not set, setting it to %s:0.0\n",
                    host);
          }
          putenv(display);
#ifndef UTMP_NO_ADDR
       } else if (utmp_entry->ut_addr) {
         struct hostent *he;
         if((he = gethostbyaddr((const char*)&utmp_entry->ut_addr,
           sizeof(utmp_entry->ut_addr), AF_INET))) {
           sprintf(display, "DISPLAY=%s:0.0", he->h_name);
           fprintf(stderr, "*** DISPLAY not set, setting it to %s:0.0\n",
           he->h_name);
           putenv(display);
         }
#endif
       }
       delete [] host;
       // display cannot be deleted otherwise the env var is deleted too
     }
     free(gUtmpContents);
   }

 }
  //printf("SetDisplay return \n");
  return;
}



static int ReadUtmp() {
  FILE  *utmp;
  struct stat file_stats;
  size_t n_read, size;

  gUtmpContents = 0;

  utmp = fopen(UTMP_FILE, "r");
  if(!utmp)
    return 0;

  fstat(fileno(utmp), &file_stats);
  size = file_stats.st_size;
  if(size <= 0) {
    fclose(utmp);
    return 0;
  }

  gUtmpContents = (STRUCT_UTMP *) malloc(size);
  if(!gUtmpContents) {
    fclose(utmp);
    return 0;
  }

  n_read = fread(gUtmpContents, 1, size, utmp);
  if(!ferror(utmp)) {
    if(fclose(utmp) != EOF && n_read == size)
      return size / sizeof(STRUCT_UTMP);
  } else
    fclose(utmp);

  free(gUtmpContents);
  gUtmpContents = 0;
  return 0;
}

static STRUCT_UTMP *SearchEntry(int n, const char *tty) {
  STRUCT_UTMP *ue = gUtmpContents;

  while (n--) {
    if(ue->ut_name[0] && !strncmp(tty, ue->ut_line, sizeof(ue->ut_line)))
      return ue;
      ue++;
   }
   return 0;
}

*/
