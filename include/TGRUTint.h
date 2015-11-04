#ifndef TGRUTINT_H
#define TGRUTINT_H

#include <string>
#include <queue>

#ifndef __CINT__
#include <condition_variable>
#include <mutex>
#include <memory>
#endif

#include <TSystem.h>
#include <TSysEvtHandler.h>
#include <TRint.h>
#include <TEnv.h>
#include <TChain.h>

#include "TGRUTServer.h"
#include "TPipeline.h"

extern TObject* gResponse;

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
  TString ReverseObjectSearch(TString&);

  Int_t TabCompletionHook(char* buf, int* pLoc, std::ostream& out);

  TFile* OpenRootFile(const std::string& filename,TChain *chain=0);

  int GetNPipelines();
  TPipeline* GetPipeline(int i);

public:
  TObject* DelayedProcessLine(std::string message);
  //GUI interface commands;
  void OpenFileDialog();
  void DefaultFunction();

  void DelayedProcessLine_ProcessItem();

  void HandleFile(const std::string& filename);

 private:
  void RunMacroFile(const std::string& filename);
  void SetupPipeline();

private:
#ifndef __CINT__
  std::mutex fCommandListMutex;
  std::mutex fResultListMutex;
  std::mutex fCommandWaitingMutex;
  std::condition_variable fNewResult;
#endif
  TTimer* fGuiTimer;

  TTimer* fCommandTimer;
  TGRUTServer *fCommandServer;
  std::queue<std::string> fLinesToProcess;
  std::queue<TObject*> fCommandResults;

  int fRootFilesOpened;

  bool fIsTabComplete;

  TPipeline* fPipeline;

  void Init();
  void ApplyOptions();
  void LoadGRootGraphics();

  ClassDef(TGRUTint,0);
};


int GetNPipelines();
TPipeline* GetPipeline(int i);

class TGRUTInterruptHandler : public TSignalHandler {
public:
  TGRUTInterruptHandler():TSignalHandler(ESignals::kSigInterrupt,false) { }
  bool Notify();
  ClassDef(TGRUTInterruptHandler,0)
};

#endif
