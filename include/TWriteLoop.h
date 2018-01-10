#ifndef _TWRITELOOP_H_
#define _TWRITELOOP_H_

#include <map>

#include "TClass.h"
#include "TTree.h"

#include "StoppableThread.h"
#include "ThreadsafeQueue.h"
#include "TUnpackedEvent.h"

class TWriteLoop : public StoppableThread {
public:
  static TWriteLoop* Get(std::string name="", std::string output_filename="");

  virtual ~TWriteLoop();

#ifndef __CINT__
  std::shared_ptr<ThreadsafeQueue<TUnpackedEvent*> >& InputQueue() { return input_queue; }
  std::shared_ptr<ThreadsafeQueue<TUnpackedEvent*> >& OutputQueue() { return output_queue; }
#endif

  virtual void ClearQueue();

  void Write();

  size_t GetItemsPushed()  { return items_handled; }
  size_t GetItemsPopped()  { return 0; }
  size_t GetItemsCurrent() { return 0;      }
  size_t GetRate()         { return 0; }

protected:
  bool Iteration();

private:
  TWriteLoop(std::string name, std::string output_file);
  void AddBranch(TClass* cls);

  void WriteEvent(TUnpackedEvent& event);
  TFile* output_file;
  TTree* event_tree;
  std::map<TClass*, TDetector**> det_map;
  std::map<TClass*, TDetector*> default_dets;

  size_t items_handled;

#ifndef __CINT__
  std::shared_ptr<ThreadsafeQueue<TUnpackedEvent*> > input_queue;
  std::shared_ptr<ThreadsafeQueue<TUnpackedEvent*> > output_queue;
#endif

  ClassDef(TWriteLoop, 0);
};


#endif /* _TWRITELOOP_H_ */
