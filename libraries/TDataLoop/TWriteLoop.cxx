#include "TWriteLoop.h"

#include "TFile.h"

#include "TPreserveGDirectory.h"

TWriteLoop::TWriteLoop(ThreadsafeQueue<TUnpackedEvent*>& input_queue, std::string filename)
  : input_queue(input_queue), in_learning_phase(true), learning_phase_length(1000) {
  TPreserveGDirectory(preserve);
  TFile* output_file = new TFile(filename.c_str(), "RECREATE");
  event_tree = new TTree("EventTree","EventTree");
  //scaler_tree = new TTree("ScalerTree","ScalerTree");
}

TWriteLoop::~TWriteLoop() {
  if(in_learning_phase){
    EndLearningPhase();
  }

  event_tree->GetCurrentFile()->Close();

  for(auto& elem : det_map){
    delete *elem.second;
    delete elem.second;
  }
}

bool TWriteLoop::Iteration() {
  TUnpackedEvent* event = NULL;
  int error = input_queue.Pop(event);

  if(error && input_queue.IsClosed()){
    return false;
  }

  if(!error) {
    HandleEvent(event);
  }
  return true;
}

void TWriteLoop::HandleEvent(TUnpackedEvent* event) {
  if(in_learning_phase) {
    LearningPhase(event);
    if(learning_queue.size() > learning_phase_length){
      EndLearningPhase();
    }
  } else {
    WriteEvent(event);
  }
}

void TWriteLoop::LearningPhase(TUnpackedEvent* event) {
  for(auto det : event->GetDetectors()) {
    TClass* cls = det->IsA();
    if(!det_map.count(cls)){
      TDetector** det = new TDetector*;
      *det = (TDetector*)cls->New();
      det_map[cls] = det;
      event_tree->Branch(cls->GetName(), cls->GetName(), det);
    }
  }
  learning_queue.push_back(event);
}

void TWriteLoop::EndLearningPhase() {
  in_learning_phase = false;
  for(auto event : learning_queue){
    WriteEvent(event);
  }
  learning_queue.clear();
}

void TWriteLoop::WriteEvent(TUnpackedEvent* event) {
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
  delete event;
}
