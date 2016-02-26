#include "TChainLoop.h"

#include "TClass.h"
#include "TFile.h"
#include "TThread.h"

#include "TGRUTint.h"
#include "THistogramLoop.h"

#include <chrono>
#include <thread>

#include "TDetector.h"
#include "TGretina.h"

//#include "TPreserveGDirectory.h"

TChainLoop* TChainLoop::Get(std::string name,TChain *chain){
  if(name.length()==0){
    name = "chain_loop";
  }

  StoppableThread* thread = StoppableThread::Get(name);
  if(!thread){
    if(!chain && !gChain){
      return 0;
    } else if(!chain) {
      chain = gChain;
    }
    thread = new TChainLoop(name,chain);
  }
  return dynamic_cast<TChainLoop*>(thread);
}

TChainLoop::TChainLoop(std::string name, TChain *chain)
  : StoppableThread(name),
    fEntriesRead(0), fEntriesTotal(chain->GetEntries()),
    input_chain(chain), hist_loop(0), fSelfStopping(true) {
    SetupChain();
}

TChainLoop::~TChainLoop() {

}

int TChainLoop::SetupChain() {
  if(!input_chain)
    return 0;
  TObjArray *array = input_chain->GetListOfBranches();
  for(int x=0;x<array->GetSize();x++) {
    TBranch *b = (TBranch*)array->At(x);
    if(b) {
      TClass *c = TClass::GetClass(b->GetName());
      if(c) {
        printf("Found  %s!\n",b->GetName());
        TDetector** det = new TDetector*;
	*det = NULL;
        det_map[c] = det;
        input_chain->SetBranchAddress(b->GetName(),det_map[c]);
      }
    }
  }
  return 0;
}

std::string TChainLoop::Status() {
  return Form("Event: %ld / %ld", fEntriesRead, fEntriesTotal);
}


void TChainLoop::Restart() {
  std::lock_guard<std::mutex> lock(restart_mutex);
  fEntriesRead = 0;
  return;
}

void TChainLoop::OnEnd() {
  if(hist_loop){
    hist_loop->SendStop();
  }
}

bool TChainLoop::Iteration() {
  if(fEntriesRead >= fEntriesTotal){
    if(fSelfStopping) {
      return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return true;
  }

  for(auto& elem : det_map){
    *elem.second = (TDetector*)elem.first->New();
  }
  input_chain->GetEntry(fEntriesRead++);

  TUnpackedEvent* event = new TUnpackedEvent;
  for(auto& elem : det_map){
    event->AddDetector(*elem.second);
  }

  if(hist_loop){
    hist_loop->Push(event);
  } else {
    delete event;
  }

  //std::cout << "hit = " << (det_map[TGretina::Class()]) << std::endl;
  //std::cout << "size = " << det_map.size() << std::endl;
  //std::map<TClass*,TDetector**>::iterator it;
  //TUnpackedEvent *event = new TUnpackedEvent();
  //for(int it=det_map.begin();it!=det_map.end();i++) {
    //TDetector *det = new TDetector(*it->second)
    //event->AddDetector(*(it->second));

  //}

  //it = det_map.begin();
  //std::cout << TGretina::Class() << std::endl;
  //std::cout << it->first << std::endl;
  //std::cout << det_map[TGretina::Class()] << std::endl;



/*
  {
    std::lock_guard<std::mutex> lock(input_queue_mutex);
    for(auto& queue : input_queues){
      int size = queue->Pop(event,0);
      if(size >= 0) {
        HandleEvent(event);
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
  */
  return true;


}

/*
void TChainLoop::HandleEvent(TUnpackedEvent* event) {
  if(in_learning_phase) {
    LearningPhase(event);
    learning_phase_size++;
    if(learning_queue.size() > learning_phase_length){
      EndLearningPhase();
    }
  } else {
    ChainEvent(event);
  }
}


void TChainLoop::LearningPhase(TUnpackedEvent* event) {
  for(auto det : event->GetDetectors()) {
    TClass* cls = det->IsA();
    if(!det_map.count(cls)){
      TThread::Lock();
      std::cout << "LearningPhase has locked" << std::endl;
      TDetector** det = new TDetector*;
      *det = (TDetector*)cls->New();
      std::cout << "Made detector" << std::endl;
      det_map[cls] = det;
      // TODO: Place this mutex here
      event_tree->Branch(cls->GetName(), cls->GetName(), det);
      TThread::UnLock();
      std::cout << "LearningPhase has unlocked" << std::endl;
    }
  }
  learning_queue.push_back(event);
}

void TChainLoop::EndLearningPhase() {
  in_learning_phase = false;
  for(auto event : learning_queue){
    ChainEvent(event);
    learning_phase_size--;
  }
  learning_queue.clear();
}
*/

/*
void TChainLoop::ChainEvent(TUnpackedEvent* event) {
  event_tree_size++;
  // Clear pointers from previous writes.
  for(auto& elem : det_map){
    *elem.second = NULL;
  }

  // Load current events
  for(auto det : event->GetDetectors()) {
    TClass* cls = det->IsA();
    *det_map.at(cls) = det;
  }

  // Fill
  event_tree->Fill();
  if(hist_loop)
    hist_loop->Push(event);
  else
    delete event;
}
*/
