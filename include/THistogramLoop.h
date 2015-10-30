#ifndef _THISTOGRAMLOOP_H_
#define _THISTOGRAMLOOP_H_

#include <string>

#include "StoppableThread.h"
#include "TCompiledHistograms.h"
#include "ThreadsafeQueue.h"

class THistogramLoop : public StoppableThread {
public:
  THistogramLoop(ThreadsafeQueue<TUnpackedEvent*>& input_queue,
                 ThreadsafeQueue<TUnpackedEvent*>& output_queue);

  THistogramLoop(ThreadsafeQueue<TUnpackedEvent*>& input_queue,
                 ThreadsafeQueue<TUnpackedEvent*>& output_queue,
                 std::string libname);

  std::string GetLibraryName() const;
  void ClearHistograms();

  void Write();

protected:
  bool Iteration();

private:
  TCompiledHistograms compiled_histograms;

  ThreadsafeQueue<TUnpackedEvent*>& input_queue;
  ThreadsafeQueue<TUnpackedEvent*>& output_queue;
};

#endif /* _THISTOGRAMLOOP_H_ */
