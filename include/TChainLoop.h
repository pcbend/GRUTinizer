#ifndef _TCHAINLOOP_H_
#define _TCHAINLOOP_H_

#ifndef __CINT__
#include <atomic>
#include <condition_variable>
#include <thread>
#endif


#include <map>
#include <vector>

#include "TChain.h"
#include "TClass.h"
#include "TDirectory.h"

#include "TUnpackingLoop.h"
#include "StoppableThread.h"
#include "ThreadsafeQueue.h"
#include "TUnpackedEvent.h"

class THistogramLoop;

class TChainLoop : public StoppableThread {
public:
  static TChainLoop* Get(std::string name="",TChain *chain=0);

  virtual ~TChainLoop();

  //void SetLearningPhaseLength(int length) { learning_phase_length = length; }
  //int GetLearningPhaseLength(int length) const { return learning_phase_length; }

  size_t GetItemsPushed()  { return fEntriesRead;   }
  size_t GetItemsPopped()  { return 0; }
  size_t GetItemsCurrent() { return fEntriesTotal;      }
  size_t GetRate()         { return 0; }

  bool AttachHistogramLoop(THistogramLoop *loop) {hist_loop = loop; return loop; }

  virtual void OnEnd();
  virtual std::string Status();

  void SetSelfStopping(bool self_stopping) { fSelfStopping = self_stopping; }
  bool GetSelfStopping() const { return fSelfStopping; }
  void Restart();

protected:
  bool Iteration();

private:
  TChainLoop(std::string name, TChain *chain);

  long fEntriesRead;
  long fEntriesTotal;

  //void HandleEvent(TUnpackedEvent* event);
  //void LearningPhase(TUnpackedEvent* event);
  //void EndLearningPhase();
  //void ChainEvent(TUnpackedEvent* event);

  TChain *input_chain;
  THistogramLoop *hist_loop;

  bool fSelfStopping;

  //std::mutex input_queue_mutex;
  //std::vector<TUnpackingLoop*> input_queues;
  //TTree* event_tree;

  int SetupChain();
  std::map<TClass*, TDetector**> det_map;
  //std::vector<TUnpackedEvent*> learning_queue;

#ifndef __CINT__
  std::mutex restart_mutex;
#endif

  ClassDef(TChainLoop, 0);
};


#endif /* _TWRITELOOP_H_ */
