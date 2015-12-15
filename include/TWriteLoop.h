#ifndef _TWRITELOOP_H_
#define _TWRITELOOP_H_

#ifndef __CINT__
#include <atomic>
#endif

#include <map>
#include <vector>

#include "TClass.h"
#include "TDirectory.h"
#include "TTree.h"

#include "TUnpackingLoop.h"
#include "StoppableThread.h"
#include "ThreadsafeQueue.h"
#include "TUnpackedEvent.h"

class THistogramLoop;

class TWriteLoop : public StoppableThread {
public:
  static TWriteLoop* Get(std::string name="", std::string output_filename="");

  virtual ~TWriteLoop();

  void Connect(TUnpackingLoop* input_queue);

  void SetLearningPhaseLength(int length) { learning_phase_length = length; }
  int GetLearningPhaseLength(int length) const { return learning_phase_length; }

  void Write();

  bool InLearningPhase() { return in_learning_phase; }

  size_t GetItemsPushed()  { return event_tree->GetEntries();   }
  size_t GetItemsPopped()  { return 0; }
  size_t GetItemsCurrent() { return learning_queue.size();      }
  size_t GetRate()         { return 0; }

  bool AttachHistogramLoop(THistogramLoop *loop) {hist_loop = loop; return loop; }

protected:
  bool Iteration();

private:
  TWriteLoop(std::string name, std::string output_file);

#ifndef __CINT__
  void HandleEvent(TUnpackedEvent* event);
  void LearningPhase(TUnpackedEvent* event);
  void EndLearningPhase();
  void WriteEvent(TUnpackedEvent* event);

  THistogramLoop *hist_loop;
  TFile* output_file;

  std::mutex input_queue_mutex;
  std::vector<TUnpackingLoop*> input_queues;
  TTree* event_tree;

  std::atomic_int event_tree_size;

  std::map<TClass*, TDetector**> det_map;
  std::vector<TUnpackedEvent*> learning_queue;
  std::atomic_int learning_phase_size;
  std::atomic_bool in_learning_phase;
  int learning_phase_length;
#endif

  ClassDef(TWriteLoop, 0);
};


#endif /* _TWRITELOOP_H_ */
