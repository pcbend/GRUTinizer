#ifndef _TWRITELOOP_H_
#define _TWRITELOOP_H_

#include <map>
#include <vector>

#include "TClass.h"
#include "TTree.h"

#include "StoppableThread.h"
#include "ThreadsafeQueue.h"
#include "TUnpackedEvent.h"

class TWriteLoop : public StoppableThread {
public:
  TWriteLoop(ThreadsafeQueue<TUnpackedEvent*>& input_queue, std::string filename);
  virtual ~TWriteLoop();

  void SetLearningPhaseLength(int length) { learning_phase_length = length; }
  int GetLearningPhaseLength(int length) const { return learning_phase_length; }

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
  bool in_learning_phase;
  int learning_phase_length;
};


#endif /* _TWRITELOOP_H_ */
