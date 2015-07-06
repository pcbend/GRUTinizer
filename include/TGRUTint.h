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

      void Init();


   ClassDef(TGRUTint,0)
};


class TGRUTInterruptHandler : public TSignalHandler {
   public:
      TGRUTInterruptHandler():TSignalHandler(ESignals::kSigInterrupt,false) { }
      bool Notify();
   ClassDef(TGRUTInterruptHandler,0)
};

#endif
