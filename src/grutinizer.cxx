#include "Globals.h"

#include <cstdio>
#include <ctime>
#include <string>
#include <unistd.h>
#include <limits.h>

#include "TEnv.h"
#include "TPluginManager.h"
#include "TPython.h"
#include "TThread.h"

#include "TGRUTint.h"
#include "TGRUTUtilities.h"

void SetGRUTPluginHandlers();

void LoadGRUTEnv() {
  // Set the GRUTSYS variable based on the executable path.
  // If GRUTSYS has already been defined, don't overwrite.
  if(!getenv("GRUTSYS")){
    char buff[PATH_MAX+1];
    size_t len = readlink("/proc/self/exe", buff, sizeof(buff)-1);
    buff[len] = '\0';
    std::string exe_path = buff;
    exe_path = exe_path.substr(0, exe_path.find_last_of('/')-4);
    setenv("GRUTSYS", exe_path.c_str(), 0);
  }

  // Load $GRUTSYS/.grutrc
  std::string grut_path = Form("%s/.grutrc",getenv("GRUTSYS"));
  gEnv->ReadFile(grut_path.c_str(),kEnvChange);

  // Load $HOME/.grutrc
  grut_path = Form("%s/.grutrc",getenv("HOME"));
  if(file_exists(grut_path.c_str())){
    gEnv->ReadFile(grut_path.c_str(),kEnvChange);
  }

  grut_path = Form("%s/.grutrc",getenv("PWD"));
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
  LoadGRUTEnv();
  SetGRUTPluginHandlers();
  AclicUseCpp11();
  // This turns on both the Cint Mutex and gROOT Mutex,
  // with-out it, you are taking thread into your own hands
  // with-it, you can use TThread::Lock/UnLock around possible hazards.
  TThread::Initialize();
  TGRUTint *input = 0;

  //Create an instance of the grut interpreter so that we can run root-like interpretive mode
  input = TGRUTint::instance(argc,argv);
  //Run the code!
  input->Run(false);
  return 0;
}

void SetGRUTPluginHandlers() {
  gPluginMgr->AddHandler("TGuiFactory","root","GROOTGuiFactory","Gui","GROOTGuiFactory()");
  gPluginMgr->AddHandler("TBrowserImp","GRootBrowser","GRootBrowser",
                         "Gui","NewBrowser(TBrowser *,const char *,Int_t,Int_t,UInt_t,UInt_t");
  gPluginMgr->AddHandler("TBrowserImp","GRootBrowser","GRootBrowser",
                         "Gui","NewBrowser(TBrowser *,const char *,Int_t,Int_t");
}
