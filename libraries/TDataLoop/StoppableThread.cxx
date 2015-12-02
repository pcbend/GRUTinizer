#include "StoppableThread.h"

#include <iostream>
#include <fstream>

#include <TString.h>

std::map<std::string,StoppableThread*> StoppableThread::fthreadmap;
bool StoppableThread::status_thread_on = false;
std::thread StoppableThread::status_thread;


int StoppableThread::GetNThreads() { return fthreadmap.size(); }

StoppableThread::StoppableThread(std::string name)
  : fname(name), running(true), paused(true) {
  //TODO: check if a thread already exists and delete?
  fthreadmap.insert(std::make_pair(fname,this));
  thread = std::thread(&StoppableThread::Loop, this);
  if(!status_thread_on)
    start_status_thread();
  //items_in  = 0;
  //items_out = 0;
  //rate      = 0;
  
}

StoppableThread *StoppableThread::Get(std::string name) {
  StoppableThread *mythread = 0;
  if(fthreadmap.count(name)) {
    mythread = fthreadmap.at(name);
  }
  return mythread;
}


StoppableThread::~StoppableThread() {
  Stop();
  Join();
  if(fthreadmap.count(fname)) {
     fthreadmap.erase(fname);
  }
  if(fthreadmap.size()==0) {
    status_thread_on = false;
    status_thread.join();
  }
}

void StoppableThread::Resume() {
  if(running) {
    paused = false;
    paused_wait.notify_one();
  }
}

void StoppableThread::Pause() {
  if(running) {
    paused = true;
  }
}

void StoppableThread::Stop() {
  running = false;
  paused = false;
  paused_wait.notify_one();
}

bool StoppableThread::IsRunning() {
  return running;
}

bool StoppableThread::IsPaused() {
  return paused;
}

void StoppableThread::Join() {
  thread.join();
}

void StoppableThread::Loop() {
  while(running){
    {
      std::unique_lock<std::mutex> lock(pause_mutex);
      while(paused){
        paused_wait.wait(lock);
      }
    }
    bool success = Iteration();
    if(!success){
      running = false;
      break;
    }
  }
}

void StoppableThread::Print() {
  printf("%i Threads:\n",GetNThreads());
  int counter = 0;
  for(auto it=fthreadmap.begin();it!=fthreadmap.end();it++) { 
    printf("  %i\t%s @ 0x%08x\n",counter,it->first.c_str(),it->second);
    counter++;
  }
}


void StoppableThread::start_status_thread() {
  status_thread_on = true;
  status_thread = std::thread(StoppableThread::status_out_loop);
  //status_thread.detach();
} 

void StoppableThread::status_out_loop() {
  while(status_thread_on) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    status_out();
  }
 std::ofstream outfile(Form("%s/.grut_thread",getenv("GRUTSYS")));
 outfile << "---------------------------------------------------------------\n"; 
 outfile << "---------------------------------------------------------------\n";
}

void StoppableThread::status_out() {

  std::ofstream outfile(Form("%s/.grut_thread",getenv("GRUTSYS")));
  outfile << "---------------------------------------------------------------\n"; // 64 -.
  for(auto it = fthreadmap.begin();it!=fthreadmap.end();it++) {
    StoppableThread *thread = it->second;
    outfile << "- " << thread->Name() << std::string(64-3-thread->Name().length(),' ') << "-\n";
    outfile << "- " << std::string(40,' ') << "items_in:  " << thread->GetItemsIn()  << "\n";
    outfile << "- " << std::string(40,' ') << "items_out: " << thread->GetItemsOut() << "\n";
    outfile << "- " << std::string(40,' ') << "rate:      " << thread->GetRate()     << "\n";
    outfile << "---------------------------------------------------------------\n"; // 64 -.
  }
  outfile << "---------------------------------------------------------------\n"; // 64 -.


};


