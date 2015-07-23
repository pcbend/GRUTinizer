#include "TDataLoop.h"

#include "RawDataQueue.h"
#include "TRawFile.h"

TDataLoop* TDataLoop::item = NULL;

TDataLoop* TDataLoop::Get() {
  if(!item){
    item = new TDataLoop;
  }
  return item;
}

TDataLoop::TDataLoop()
  : running(false) {
  queue = new RawDataQueue;
}

TDataLoop::~TDataLoop(){
  delete queue;
}

void TDataLoop::Initialize() {
  initialized = true;
}

void TDataLoop::Start(){
  if(!initialized){
    Initialize();
  }
}

void TDataLoop::ProcessFile(TRawFileIn* infile) {
  if(!running){
    this->infile = infile;
    std::cout << "File error: " << infile->GetLastError() << std::endl;
    running = true;
    read_thread = std::thread(&TDataLoop::ReadLoop, this);
  }
}

void TDataLoop::Pause() {
  paused = true;
}

void TDataLoop::Resume() {
  paused = false;
  paused_wait.notify_all();
}

void TDataLoop::Stop() {
  if(running) {
    Resume();
    running = false;
    read_thread.join();
    Finalize();
  }
}

void TDataLoop::Finalize() {
  initialized = false;
}


void TDataLoop::ReadLoop() {
  std::cout << "Read loop starting" << std::endl;
  while(running){
    std::cout << "In read loop" << std::endl;
    std::unique_lock<std::mutex> lock(pause_mutex);
    while(!paused){
      paused_wait.wait(lock);
    }

    std::cout << "I am not paused" << std::endl;

    Iteration();
  }
  std::cout << "End of read loop" << std::endl;
}

void TDataLoop::Iteration() {
  std::cout << "I am iterating" << std::endl;
  TRawEvent* evt = new TRawEvent;
  infile->Read(evt);
  queue->Push(evt);
}

void TDataLoop::PrintQueue(){
  queue->Print();
}
