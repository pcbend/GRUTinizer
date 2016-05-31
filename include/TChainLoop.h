#ifndef _TCHAINLOOP_H_
#define _TCHAINLOOP_H_

#ifndef __CINT__
#include <atomic>
#endif

#include <map>

#include "TChain.h"
#include "TClass.h"

#include "TUnpackingLoop.h"
#include "StoppableThread.h"
#include "ThreadsafeQueue.h"
#include "TUnpackedEvent.h"

class TUnpackedEvent;

class TChainLoop : public StoppableThread {
public:
  static TChainLoop* Get(std::string name="",TChain *chain=0);
  virtual ~TChainLoop();

#ifndef __CINT__
  std::shared_ptr<ThreadsafeQueue<TUnpackedEvent*> >& OutputQueue() { return output_queue; }
#endif

  size_t GetItemsPushed()  { return fEntriesRead;   }
  size_t GetItemsPopped()  { return 0; }
  size_t GetItemsCurrent() { return fEntriesTotal;      }
  size_t GetRate()         { return 0; }

  virtual std::string Status();
  virtual void ClearQueue();

  virtual void OnEnd();

  void SetSelfStopping(bool self_stopping) { fSelfStopping = self_stopping; }
  bool GetSelfStopping() const { return fSelfStopping; }
  void Restart();

protected:
  bool Iteration();

private:
  TChainLoop(std::string name, TChain *chain);

#ifndef __CINT__
  std::atomic_long fEntriesRead;
#endif
  long fEntriesTotal;

  TChain *input_chain;
#ifndef __CINT__
  std::shared_ptr<ThreadsafeQueue<TUnpackedEvent*> > output_queue;
#endif

  bool fSelfStopping;

  int SetupChain();
  std::map<TClass*, TDetector**> det_map;

  ClassDef(TChainLoop, 0);
};


#endif /* _TWRITELOOP_H_ */
