#include "THistogramLoop.h"

#include "TPreserveGDirectory.h"

THistogramLoop::THistogramLoop(ThreadsafeQueue<TUnpackedEvent*>& input_queue,
                               ThreadsafeQueue<TUnpackedEvent*>& output_queue,
                               TDirectory* dir)
  : input_queue(input_queue), output_queue(output_queue),
    output_dir(dir) {
  compiled_histograms.SetDefaultDirectory(dir);
}

bool THistogramLoop::Iteration() {
  TUnpackedEvent* event = NULL;
  int error = input_queue.Pop(event);

  if (error){
    return true;
  }

  if(event) {
    compiled_histograms.Fill(*event);
    output_queue.Push(event);
  }
  return true;
}

void THistogramLoop::ClearHistograms() {
  compiled_histograms.ClearHistograms();
}

void THistogramLoop::Write() {
  TPreserveGDirectory preserve;
  if(output_dir){
    output_dir->cd();
  }
  compiled_histograms.Write();
}

void THistogramLoop::LoadLibrary(std::string library) {
  compiled_histograms.Load(library);
}

std::string THistogramLoop::GetLibraryName() const {
  return compiled_histograms.GetLibraryName();
}
