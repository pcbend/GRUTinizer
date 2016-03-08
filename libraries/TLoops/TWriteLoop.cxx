#include "TWriteLoop.h"

#include "TFile.h"
#include "TThread.h"

#include "GValue.h"
#include "TChannel.h"
#include "THistogramLoop.h"
#include "TS800.h"

#include <chrono>
#include <thread>

//#include "TPreserveGDirectory.h"

TWriteLoop* TWriteLoop::Get(std::string name, std::string output_filename){
  if(name.length()==0){
    name = "write_loop";
  }

  StoppableThread* thread = StoppableThread::Get(name);
  if(!thread){
    if(output_filename.length()==0){
      output_filename = "temp.root";
    }
    thread = new TWriteLoop(name,output_filename);
  }

  return dynamic_cast<TWriteLoop*>(thread);
}

TWriteLoop::TWriteLoop(std::string name, std::string output_filename)
  : StoppableThread(name),
    hist_loop(0), output_file(NULL), event_tree(NULL),
    items_handled(0) {

  if(output_filename != "/dev/null"){
    //TPreserveGDirectory preserve;
    output_file = new TFile(output_filename.c_str(),"RECREATE");
    event_tree = new TTree("EventTree","EventTree");
    //scaler_tree = new TTree("ScalerTree","ScalerTree");
  }
}

TWriteLoop::~TWriteLoop() {
  for(auto& elem : det_map){
    delete elem.second;
  }

  if(output_file){
    output_file->cd();
    event_tree->Write(event_tree->GetName(), TObject::kOverwrite);
    if(GValue::Size())
      GValue::Get()->Write();
    if(TChannel::Size())
      TChannel::Get()->Write();

    output_file->Close();
    output_file->Delete();
  }
}

void TWriteLoop::Connect(TUnpackingLoop* input_queue){
  if(input_queue){
    std::lock_guard<std::mutex> lock(input_queue_mutex);
    input_queues.push_back(input_queue);
  }
}

bool TWriteLoop::Iteration() {
  TUnpackedEvent* event = NULL;

  bool handled_event = false;
  bool living_parent = false;

  {
    std::lock_guard<std::mutex> lock(input_queue_mutex);
    for(auto& queue : input_queues){
      int size = queue->Pop(event,0);
      if(size >= 0) {
        WriteEvent(event);
        items_handled++;
	handled_event = true;
	living_parent = true;
      } else if (queue->IsRunning()){
	living_parent = true;
      }
    }
  }

  if(!handled_event){
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  if(!living_parent && hist_loop)
    hist_loop->SendStop();

  return living_parent || (input_queues.size()==0);
}

void TWriteLoop::Write() {
  if(output_file){
    //TPreserveGDirectory preserve;
    output_file->cd();
    event_tree->Write();
  }
}

void TWriteLoop::AddBranch(TClass* cls){
  if(!det_map.count(cls)){
    // This uses the ROOT dictionaries, so we need to lock the threads.
    TThread::Lock();

    // Make the TDetector**
    TDetector** det = new TDetector*;
    *det = (TDetector*)cls->New();
    det_map[cls] = det;

    // Make a new branch.
    TBranch* new_branch = event_tree->Branch(cls->GetName(), cls->GetName(), det);
    delete *det;
    *det = NULL;

    // Fill the new branch up to the point where the tree is filled.
    // Explanation:
    //   When TTree::Fill is called, it calls TBranch::Fill for each
    // branch, then increments the number of entries.  We may be
    // adding branches after other branches have already been filled.
    // If the S800 branch has been filled 100 times before the Gretina
    // branch is created, then the next call to TTree::Fill will fill
    // entry 101 of S800, but entry 1 of Gretina, rather than entry
    // 101 of both.
    //   Therefore, we need to fill the new branch as many times as
    // TTree::Fill has been called before.
    for(int i=0; i<event_tree->GetEntries(); i++){
      new_branch->Fill();
    }

    std::cout << "\r" << std::string(30,' ')
              << "\rAdded \"" << cls->GetName() << "\" branch" << std::endl;

    // Unlock after we are done.
    TThread::UnLock();
  }
}

void TWriteLoop::WriteEvent(TUnpackedEvent* event) {
  if(event_tree){
    // Clear pointers from previous writes.
    for(auto& elem : det_map){
      *elem.second = NULL;
    }

    // Load current events
    for(auto det : event->GetDetectors()) {
      TClass* cls = det->IsA();
      try{
        *det_map.at(cls) = det;
      } catch (std::out_of_range& e) {
        AddBranch(cls);
        *det_map.at(cls) = det;
      }
    }

    // Fill
    event_tree->Fill();
  }

  if(hist_loop)
    hist_loop->Push(event);
  else
    delete event;
}
