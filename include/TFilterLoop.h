#ifndef _TFILTERLOOP_H_
#define _TFILTERLOOP_H_

#include <string>

#include "StoppableThread.h"
#include "TCompiledFilter.h"
#include "ThreadsafeQueue.h"

class TFile;

class TFilterLoop : public StoppableThread {
public:
  static TFilterLoop* Get(std::string name="");

  ~TFilterLoop();

#ifndef __CINT__
  std::shared_ptr<ThreadsafeQueue<TUnpackedEvent*> >& InputQueue() { return input_queue; }
  std::shared_ptr<ThreadsafeQueue<TUnpackedEvent*> >& OutputQueue() { return output_queue; }
#endif

  void LoadLibrary(std::string library);
  std::string GetLibraryName() const;

  void AddCutFile(TFile* cut_file);

  virtual void ClearQueue();

  size_t GetItemsPopped()  { return output_queue->ItemsPopped(); }
  size_t GetItemsPushed()  { return output_queue->ItemsPushed(); }
  size_t GetItemsCurrent() { return output_queue->Size(); }
  size_t GetRate()         { return 0; }

protected:
  bool Iteration();
  void LoadLib(std::string libname) { compiled_filter.Load(libname); }

private:
  TFilterLoop(std::string name);

  TCompiledFilter compiled_filter;

  std::string output_filename;

#ifndef __CINT__
  std::shared_ptr<ThreadsafeQueue<TUnpackedEvent*> > input_queue;
  std::shared_ptr<ThreadsafeQueue<TUnpackedEvent*> > output_queue;
#endif

  ClassDef(TFilterLoop,0);
};

#endif /* _TFILTERLOOP_H_ */
