#ifndef _TROOTINPUTLOOP_H_
#define _TROOTINPUTLOOP_H_

#include "TClass.h"
#include "TTree.h"

#include "StoppableThread.h"
#include "TDetector.h"
#include "ThreadsafeQueue.h"
#include "TUnpackedEvent.h"

class TRootInputLoop : public StoppableThread {
public:
  TRootInputLoop(TTree* tree);
  virtual ~TRootInputLoop();

  ThreadsafeQueue<TUnpackedEvent*>& GetOutputQueue() { return output_queue; }

protected:
  bool Iteration();

private:
  void SetupBranches();

  TTree* tree;
  size_t ientry;
  size_t nentries;
  std::map<TClass*,TDetector**> det_map;
  ThreadsafeQueue<TUnpackedEvent*> output_queue;
};


#endif /* _TROOTINPUTLOOP_H_ */
