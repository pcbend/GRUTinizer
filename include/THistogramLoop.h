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
  void SetOutputFilename(const std::string& name);
  std::string GetOutputFilename() const;

  void LoadLibrary(std::string library);
  std::string GetLibraryName() const;
  void ClearHistograms();

  void Write();

  void SetReplaceVariable(const char* name, double value);
  void RemoveVariable(const char* name);
  TList* GetVariables();

  TList* GetObjects();

  size_t GetItemsPopped()  { return input_queue.ItemsPopped(); }
  size_t GetItemsPushed()  { return input_queue.ItemsPushed(); }
  size_t GetItemsCurrent() { return 0; }
  size_t GetRate()         { return 0; }

  void cd(Option_t* opt="");
  void popd();

  int Push(TUnpackedEvent *event);
  int SendStop(bool flag=true) { stopsent = flag; }

protected:
  bool Iteration();
  void LoadLib(std::string libname) { compiled_histograms.Load(libname); }

private:
  THistogramLoop(std::string name);

  TCompiledHistograms compiled_histograms;

  ThreadsafeQueue<TUnpackedEvent*>  input_queue;
  //ThreadsafeQueue<TUnpackedEvent*>& output_queue;

  void OpenFile();
  void CloseFile();

  TFile* output_file;
  TDirectory* previous_dir;
  std::string output_filename;
  bool stopsent;

  ClassDef(THistogramLoop,0);
};

#endif /* _THISTOGRAMLOOP_H_ */
