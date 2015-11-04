#include "StoppableThread.h"

#include <iostream>

StoppableThread::StoppableThread()
  : running(true), paused(true) {
  thread = std::thread(&StoppableThread::Loop, this);
}

StoppableThread::~StoppableThread() {
  Stop();
  Join();
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
