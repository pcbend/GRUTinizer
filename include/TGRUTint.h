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

#include "TChain.h"
#include "TEnv.h"
#include "TList.h"
#include "TRint.h"
#include "TSysEvtHandler.h"
#include "TSystem.h"

class TCutG;

class TChainLoop;
class TDataLoop;
class THistogramLoop;
class TRawEventSource;
class TRawFileIn;

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
  void ResetAllHistograms();
  void ResortDataFile();

  void LoadRawFile(std::string filename);

  void LoadTCutG(TCutG* cutg);
  TList *GetListOfRawFiles() { return &fOpenedRawFiles; }

  void DelayedProcessLine_Action();

protected:
  void RunMacroFile(const std::string& filename);
  void SplashPopNWait(bool flag);

private:
  Long_t DelayedProcessLine(std::string message);

  TTimer* fKeepAliveTimer;
#ifndef __CINT__
  std::thread::id main_thread_id;
#endif

  int fRootFilesOpened;
  int fRawFilesOpened;

  bool fIsTabComplete;
  bool fAllowedToTerminate;

  void Init();
  void ApplyOptions();
  void LoadGRootGraphics();

  TList fOpenedRawFiles;

  // Data processing objects/methods
  void SetupPipeline();
  TRawEventSource* OpenRawSource();

  TDataLoop* fDataLoop;
  TChainLoop* fChainLoop;
  THistogramLoop* fHistogramLoop;


  ClassDef(TGRUTint,0);
};


class TGRUTInterruptHandler : public TSignalHandler {
public:
  TGRUTInterruptHandler():TSignalHandler(ESignals::kSigInterrupt,false) { }
  bool Notify();
  ClassDef(TGRUTInterruptHandler,0)
};

#endif
