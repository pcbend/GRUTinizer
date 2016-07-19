#ifndef _THISTOGRAMLOOP_H_
#define _THISTOGRAMLOOP_H_

#include <string>

#include "StoppableThread.h"
#include "TCompiledHistograms.h"
#include "ThreadsafeQueue.h"

class TFile;

class THistogramLoop : public StoppableThread {
public:
  static THistogramLoop* Get(std::string name="");

  ~THistogramLoop();

#ifndef __CINT__
  std::shared_ptr<ThreadsafeQueue<TUnpackedEvent*> >& InputQueue() { return input_queue; }
  std::shared_ptr<ThreadsafeQueue<TUnpackedEvent*> >& OutputQueue() { return output_queue; }
#endif

  void SetOutputFilename(const std::string& name);
  std::string GetOutputFilename() const;

  void LoadLibrary(std::string library);
  std::string GetLibraryName() const;
  void ClearHistograms();

  void AddCutFile(TFile* cut_file);

  void Write();

  virtual void ClearQueue();

  TList* GetObjects();
  TList* GetGates();

  size_t GetItemsPopped()  { return input_queue->ItemsPopped(); }
  size_t GetItemsPushed()  { return input_queue->ItemsPushed(); }
  size_t GetItemsCurrent() { return 0; }
  size_t GetRate()         { return 0; }

protected:
  bool Iteration();
  void LoadLib(std::string libname) { compiled_histograms.Load(libname); }

private:
  THistogramLoop(std::string name);

  TCompiledHistograms compiled_histograms;

  void OpenFile();
  void CloseFile();

  TFile* output_file;
  std::string output_filename;

#ifndef __CINT__
  std::shared_ptr<ThreadsafeQueue<TUnpackedEvent*> > input_queue;
  std::shared_ptr<ThreadsafeQueue<TUnpackedEvent*> > output_queue;
#endif

  ClassDef(THistogramLoop,0);
};

#endif /* _THISTOGRAMLOOP_H_ */
