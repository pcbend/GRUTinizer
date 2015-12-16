#ifndef TGRUTINT_H
#define TGRUTINT_H

#include <string>
#include <queue>

#ifndef __CINT__
#include <condition_variable>
#include <mutex>
#include <memory>
#include <thread>
#endif

#include <TSystem.h>
#include <TSysEvtHandler.h>
#include <TRint.h>
#include <TEnv.h>
#include <TChain.h>
#include <TList.h>

//#include "TGRUTServer.h"

class TRawFileIn;

//extern TObject* gResponse;
extern TChain *gChain;

class TGRUTint : public TRint {

  private:
    TGRUTint(int argc, char **argv, void *opts=0,int numOptions=0,
             bool noLogo = true, const char *appClassName = "grutinizer");
    static TEnv *fGRUTEnv;
    static TGRUTint *fTGRUTint;

  public:
    static TGRUTint *instance(int argc=0,char **argv=0,void *opts=0,int numOptions=-1,
                              bool noLogo=true,const char *appClassName="grutinizer");
    virtual ~TGRUTint();
    virtual void Terminate(Int_t status = 0);
    Long_t ProcessLine(const char* line, Bool_t sync=kTRUE,Int_t *error=0);

    //TString ReverseObjectSearch(TString&);
    //TObject* ObjectAppended(TObject* obj);

    Int_t TabCompletionHook(char* buf, int* pLoc, std::ostream& out);
    TFile* OpenRootFile(const std::string& filename, Option_t* opt="");
    TRawFileIn* OpenRawFile(const std::string& filename);

public:
  //void HandleFile(const std::string& filename);
  void DelayedProcessLine_Action();


 protected:
   void RunMacroFile(const std::string& filename);
   void SplashPopNWait(bool flag);
   void CreateDataLoop();

private:
  Long_t DelayedProcessLine(std::string message);

  //TTimer* fGuiTimer;

  //TTimer* fCommandTimer;
#ifndef __CINT__
  std::thread::id main_thread_id;
#endif

  int fRootFilesOpened;
  int fRawFilesOpened;

  //TObject* fNewChild;
  bool fIsTabComplete;
  //TGRUTServer *fCommandServer;

  TChain* fChain;

  void Init();
  void ApplyOptions();
  void LoadGRootGraphics();
  void LoadDetectorClasses();

  public:
    TList *GetListOfRawFiles() { return &fOpenedRawFiles; }

  private:
    TList fOpenedRawFiles;



  ClassDef(TGRUTint,0);
};


class TGRUTInterruptHandler : public TSignalHandler {
public:
  TGRUTInterruptHandler():TSignalHandler(ESignals::kSigInterrupt,false) { }
  bool Notify();
  ClassDef(TGRUTInterruptHandler,0)
};

#endif
