#ifndef _TWRITELOOP_H_
#define _TWRITELOOP_H_

#ifndef __CINT__
#include <atomic>
#endif

#include <map>
#include <vector>

#include "TClass.h"
#include "TDirectory.h"
#include "TTree.h"

#include "TUnpackingLoop.h"
#include "StoppableThread.h"
#include "ThreadsafeQueue.h"
#include "TUnpackedEvent.h"

class THistogramLoop;

class TWriteLoop : public StoppableThread {
public:
  static TWriteLoop* Get(std::string name="", std::string output_filename="");

  virtual ~TWriteLoop();

  void Connect(TUnpackingLoop* input_queue);

  void Write();

  size_t GetItemsPushed()  { return items_handled; }
  size_t GetItemsPopped()  { return 0; }
  size_t GetItemsCurrent() { return 0;      }
  size_t GetRate()         { return 0; }

  bool AttachHistogramLoop(THistogramLoop *loop) {hist_loop = loop; return loop; }

protected:
  bool Iteration();

private:
  TWriteLoop(std::string name, std::string output_file);
  void AddBranch(TClass* cls);

  //ThreadsafeQueue<TUnpackedEvent*> input_queue;

#ifndef __CINT__
  void WriteEvent(TUnpackedEvent* event);
  THistogramLoop *hist_loop;
  TFile* output_file;
  std::mutex input_queue_mutex;
  std::vector<TUnpackingLoop*> input_queues;
  TTree* event_tree;
  std::map<TClass*, TDetector**> det_map;

  size_t items_handled;
#endif

  ClassDef(TWriteLoop, 0);
};


#endif /* _TWRITELOOP_H_ */
