#ifndef GRUTNOTIFIER__H__
#define GRUTNOTIFIER__H__

#include "TNamed.h"

#include "GRootCommands.h"

class GrutNotifier : public TNamed {
  public:
    static GrutNotifier *Get();
    virtual ~GrutNotifier();

    virtual void Print(Option_t *opt="") const { }
    virtual void Clear(Option_t *opt="") { }
    virtual bool Notify();

  private:
    GrutNotifier();
    static GrutNotifier *fGrutNotifier;

  ClassDef(GrutNotifier,0)
};

#endif

