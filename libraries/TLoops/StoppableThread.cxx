#include "StoppableThread.h"

#include <iostream>
#include <fstream>

#include <TString.h>

#include "TDataLoop.h"

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

void StoppableThread::StopAll() {
  std::cout << "Stopping status thread" << std::endl;
  stop_status_thread();

  std::cout << "Stopping each thread" << std::endl;
  for(auto& elem : fthreadmap){
    std::cout << "Stopping thread " << elem.first << std::endl;
    StoppableThread* thread = elem.second;
    thread->Stop();
  }

  for(auto& elem : fthreadmap){
    std::cout << "Joining thread " << elem.first << std::endl;
    StoppableThread* thread = elem.second;
    thread->Join();
  }

  for(auto& elem : fthreadmap){
    std::cout << "Deleting thread " << elem.first << std::endl;
    StoppableThread* thread = elem.second;
    delete thread;
  }

  std::cout << "Last status" << std::endl;
  status_out();
  std::cout << "End of function" << std::endl;
}

void StoppableThread::StopAllClean() {
  std::cout << "Stopping each TDataLoop" << std::endl;
  for(auto& elem : fthreadmap){
    TDataLoop* thread = dynamic_cast<TDataLoop*>(elem.second);
    if(thread){
      std::cout << "Stopping thread " << elem.first << std::endl;
      thread->Stop();
    }
  }

  for(auto& elem : fthreadmap){
    std::cout << "Joining thread " << elem.first << std::endl;
    StoppableThread* thread = elem.second;
    thread->Join();
  }

  for(auto& elem : fthreadmap){
    std::cout << "Deleting thread " << elem.first << std::endl;
    StoppableThread* thread = elem.second;
    delete thread;
  }

  std::cout << "Last status" << std::endl;
  status_out();
  std::cout << "End of function" << std::endl;
}

StoppableThread *StoppableThread::Get(std::string name) {
  StoppableThread *mythread = 0;
  if(fthreadmap.count(name)) {
    mythread = fthreadmap.at(name);
  }
  return mythread;
}


StoppableThread::~StoppableThread() {
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
    std::unique_lock<std::mutex> lock(pause_mutex);
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
  std::unique_lock<std::mutex> lock(pause_mutex);
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
      while(paused && running){
        paused_wait.wait_for(lock, std::chrono::milliseconds(100));
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
  if(!status_thread_on){
    status_thread_on = true;
    status_thread = std::thread(StoppableThread::status_out_loop);
  }
}

void StoppableThread::stop_status_thread() {
  if(status_thread_on){
    status_thread_on = false;
  }
}

void StoppableThread::join_status_thread() {
  stop_status_thread();
  status_thread.join();
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
    outfile << "- " << thread->Name() << (thread->IsRunning()?"[Live]":"[Stop]") << std::string(64-8-thread->Name().length(),' ')   << "-\n";
    outfile << "- " << std::string(40,' ') << "items_pushed:  " << thread->GetItemsPushed()  << "\n";
    outfile << "- " << std::string(40,' ') << "items_popped:  " << thread->GetItemsPopped()  << "\n";
    outfile << "- " << std::string(40,' ') << "items_current: " << thread->GetItemsCurrent() << "\n";
    outfile << "- " << std::string(40,' ') << "rate:      " << thread->GetRate()         << "\n";
    outfile << "---------------------------------------------------------------\n"; // 64 -.
  }
  outfile << "---------------------------------------------------------------\n"; // 64 -.


};
