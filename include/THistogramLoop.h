#ifndef _THISTOGRAMLOOP_H_
#define _THISTOGRAMLOOP_H_

#include <string>

#include "StoppableThread.h"
#include "TCompiledHistograms.h"
#include "ThreadsafeQueue.h"

//class TWriteLoop;

class THistogramLoop : public StoppableThread {
public:
  static THistogramLoop* Get(std::string name=""); //, TWriteLoop* input=0);

  void LoadLibrary(std::string library);
  std::string GetLibraryName() const;
  void ClearHistograms();

  void Write();

  void SetReplaceVariable(const char* name, double value);
  void RemoveVariable(const char* name);
  TList* GetVariables();


  size_t GetItemsPopped()  { return input_queue.ItemsPopped(); }
  size_t GetItemsPushed()  { return input_queue.ItemsPushed(); }
  size_t GetItemsCurrent() { return 0; }
  size_t GetRate()         { return 0; }


  int Push(TUnpackedEvent *event);
  int SendStop(bool flag=true) { stopsent = flag; }

protected:
  bool Iteration();
  void LoadLib(std::string libname) { compiled_histograms.Load(libname); }

private:
  THistogramLoop(std::string name); //, TWriteLoop* input);

  TCompiledHistograms compiled_histograms;

  ThreadsafeQueue<TUnpackedEvent*>  input_queue;
  //ThreadsafeQueue<TUnpackedEvent*>& output_queue;

  TDirectory* output_dir;
  bool stopsent;
};

#endif /* _THISTOGRAMLOOP_H_ */
