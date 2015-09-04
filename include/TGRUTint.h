#ifndef TGRUTINT_H
#define TGRUTINT_H

#include <string>
#include <queue>

#ifndef __CINT__
#include <mutex>
#include <memory>
#endif

#include <TSystem.h>
#include <TSysEvtHandler.h>
#include <TRint.h>
#include <TEnv.h>

#include "TGRUTServer.h"


class TGRUTint : public TRint {

private:
  TGRUTint(int argc, char **argv, void *opts=0,int numOptions=0,
           bool noLogo = true, const char *appClassName = "grutinizer");

  static TEnv *fGRUTEnv;
  static TGRUTint *fTGRUTint;

public:
  static TGRUTint*instance(int argc=0,char **argv=0,void *opts=0,int numOptions=-1,
                           bool noLogo=true,const char *appClassName="grutinizer");
  virtual ~TGRUTint();

  virtual void Terminate(Int_t status = 0);

  void SetListenPort(int port) { fServer.SetPort(port); }

  Long_t ProcessLine(const char* line, Bool_t sync=kFALSE,Int_t *error=0);
  TString ReverseObjectSearch(TString&);


  TObject* ObjectAppended(TObject* obj);

  Int_t TabCompletionHook(char* buf, int* pLoc, std::ostream& out);

public:
  void DelayedProcessLine(std::string message);
  //GUI interface commands;
  void OpenFileDialog();

  void DelayedProcessLine_ProcessItem();
private:
#ifndef __CINT__
  std::mutex fCommandsMutex;
#endif
  TTimer* fCommandTimer;
  std::queue<std::string> fLinesToProcess;

  TObject* fNewChild;
  bool fIsTabComplete;
  TGRUTServer fServer;

  void Init();
  void ApplyOptions();

  ClassDef(TGRUTint,0);
};


class TGRUTInterruptHandler : public TSignalHandler {
public:
  TGRUTInterruptHandler():TSignalHandler(ESignals::kSigInterrupt,false) { }
  bool Notify();
  ClassDef(TGRUTInterruptHandler,0)
};

#endif
