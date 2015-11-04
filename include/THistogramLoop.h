#ifndef _THISTOGRAMLOOP_H_
#define _THISTOGRAMLOOP_H_

#include <string>

#include "StoppableThread.h"
#include "TCompiledHistograms.h"
#include "ThreadsafeQueue.h"

class THistogramLoop : public StoppableThread {
public:
  THistogramLoop(ThreadsafeQueue<TUnpackedEvent*>& input_queue,
                 ThreadsafeQueue<TUnpackedEvent*>& output_queue,
                 TDirectory* dir = NULL);

  void LoadLibrary(std::string library);
  std::string GetLibraryName() const;
  void ClearHistograms();

  void Write();

  void SetReplaceVariable(const char* name, double value);
  void RemoveVariable(const char* name);
  TList* GetVariables();

protected:
  bool Iteration();

private:
  TCompiledHistograms compiled_histograms;

  ThreadsafeQueue<TUnpackedEvent*>& input_queue;
  ThreadsafeQueue<TUnpackedEvent*>& output_queue;

  TDirectory* output_dir;
};

#endif /* _THISTOGRAMLOOP_H_ */
