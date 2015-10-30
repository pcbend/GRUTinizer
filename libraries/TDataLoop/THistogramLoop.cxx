#include "THistogramLoop.h"

THistogramLoop::THistogramLoop(ThreadsafeQueue<TUnpackedEvent*>& input_queue,
                               ThreadsafeQueue<TUnpackedEvent*>& output_queue)
  : input_queue(input_queue), output_queue(output_queue) { }

THistogramLoop::THistogramLoop(ThreadsafeQueue<TUnpackedEvent*>& input_queue,
                               ThreadsafeQueue<TUnpackedEvent*>& output_queue,
                               std::string libname)
  : compiled_histograms(libname),
    input_queue(input_queue), output_queue(output_queue) { }

bool THistogramLoop::Iteration() {
  TUnpackedEvent* event = NULL;
  int error = input_queue.Pop(event);

  if(error && input_queue.IsClosed()){
    return false;
  } else if (error){
    return true;
  }

  if(event) {
    compiled_histograms.Fill(*event);
    output_queue.Push(event);
  }
}

void THistogramLoop::ClearHistograms() {
  compiled_histograms.ClearHistograms();
}

void THistogramLoop::Write() {
  compiled_histograms.Write();
}
