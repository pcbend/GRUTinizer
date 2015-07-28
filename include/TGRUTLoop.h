#ifndef _TGRUTLOOP_H_
#define _TGRUTLOOP_H_

#include "TNamed.h"

#include "TDataLoop.h"

// So that rootcint doesn't see this
class RawDataQueue;
class TRootOutfile;

class TGRUTLoop : public TDataLoop {
public:
  static TGRUTLoop* Get();

  virtual ~TGRUTLoop();
  virtual int ProcessEvent(TRawEvent& event);

  void PrintQueue();
  void StatusQueue();

  TRawEvent GetEvent();

  virtual void Initialize();
  virtual void Finalize();

  void PrintOutfile();

private:
  template<typename T, typename... Params>
  friend void TDataLoop::CreateDataLoop(Params&&... params);

  TGRUTLoop();
  void WriteLoop();

  RawDataQueue* queue;
  TRootOutfile* outfile;

#ifndef __CINT__
  std::thread write_thread;
#endif


  ClassDef(TGRUTLoop,0);
};

#endif /* _TGRUTLOOP_H_ */
