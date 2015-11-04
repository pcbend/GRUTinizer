#ifndef _TWRITELOOP_H_
#define _TWRITELOOP_H_

#include <atomic>
#include <map>
#include <vector>

#include "TClass.h"
#include "TDirectory.h"
#include "TTree.h"

#include "StoppableThread.h"
#include "ThreadsafeQueue.h"
#include "TUnpackedEvent.h"

class TWriteLoop : public StoppableThread {
public:
  TWriteLoop(ThreadsafeQueue<TUnpackedEvent*>& input_queue, TDirectory& dir);
  virtual ~TWriteLoop();

  void SetLearningPhaseLength(int length) { learning_phase_length = length; }
  int GetLearningPhaseLength(int length) const { return learning_phase_length; }

  void Write();

  bool InLearningPhase() { return in_learning_phase; }

protected:
  bool Iteration();

private:
  void HandleEvent(TUnpackedEvent* event);
  void LearningPhase(TUnpackedEvent* event);
  void EndLearningPhase();
  void WriteEvent(TUnpackedEvent* event);

  ThreadsafeQueue<TUnpackedEvent*>& input_queue;
  TTree* event_tree;
  //TTree* scaler_tree;

  std::map<TClass*, TDetector**> det_map;
  std::vector<TUnpackedEvent*> learning_queue;
  std::atomic_bool in_learning_phase;
  int learning_phase_length;

  TDirectory& output_dir;
};


#endif /* _TWRITELOOP_H_ */
