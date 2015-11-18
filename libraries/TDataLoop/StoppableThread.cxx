#include "StoppableThread.h"

#include <iostream>

std::map<std::string,StoppableThread*> fthreadmap;

int StoppableThread::GetNThreads() { return fthreadmap.size(); }

StoppableThread::StoppableThread(name)
  : fname(name), running(true), paused(true) {
  //TODO: check if a thread already exists and delete?
  fthreadmap.insert(std::make_pair(fname,this));
  thread = std::thread(&StoppableThread::Loop, this);
}

StoppableThread *StoppableThread::Get(std::string name) {
  StoppableThread *mythread = 0;
  if(fthreadmap.count(name)) {
    mythread = fthreadmap.at(name)
  }
  return mythread;
}


StoppableThread::~StoppableThread() {
  Stop();
  Join();
  if(fthreadmap.count(name)) {
     fthreadmap.erase(name)
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
