#ifndef TGRUTINT_H
#define TGRUTINT_H

#include <TSystem.h>
#include <TSysEvtHandler.h>
#include <TRint.h>
#include <TEnv.h>


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

  Long_t ProcessLine(const char* line, Bool_t sync=kFALSE,Int_t *error=0);
  TString ReverseObjectSearch(TString&);


  TObject* ObjectAppended(TObject* obj);

  Int_t TabCompletionHook(char* buf, int* pLoc, std::ostream& out);

private:
  TObject* fNewChild;
  bool fIsTabComplete;

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
