#ifndef GRUTNOTIFIER__H__
#define GRUTNOTIFIER__H__

#ifndef __CINT__
#include <functional>
#endif

#include "TNamed.h"

#include "GRootCommands.h"

class GrutNotifier : public TNamed {
  public:
    static GrutNotifier *Get();
    virtual ~GrutNotifier();

    virtual void Print(Option_t *opt="") const { }
    virtual void Clear(Option_t *opt="") { }
    virtual bool Notify();

    #ifdef __CINT__
    static void AddCallback(void (*func)());
    #else
    template<typename T>
      static void AddCallback(T callback) {
      Get()->callbacks.push_back(callback);
    }

    static void AddCallback(void (*func)()) {
      Get()->callbacks.push_back(func);
    }
    #endif

  private:
    GrutNotifier();

    #ifndef __CINT__
    std::vector<std::function<void()> > callbacks;
    #endif
    
    static GrutNotifier *fGrutNotifier;

  ClassDef(GrutNotifier,0)
};

#endif

