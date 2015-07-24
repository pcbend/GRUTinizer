#ifndef _TDATALOOP_H_
#define _TDATALOOP_H_

#ifndef __CINT__
#include <atomic>
#include <condition_variable>
#include <thread>
#endif

#include "TNamed.h"

class RawDataQueue;
class TRawFileIn;

class TRawEvent;

class TDataLoop : public TNamed {
public:
  static TDataLoop* Get();
  ~TDataLoop();

  void Initialize();
  void Start();
  void Pause();
  void Resume();
  void Stop();
  void Finalize();

  void ProcessFile(TRawFileIn* infile);
  void PrintQueue();
  void StatusQueue();

  bool IsPaused();

  TRawEvent* GetEvent();

private:
  static TDataLoop* item;
  TDataLoop();
  TDataLoop(const TDataLoop& other) { AbstractMethod("SHOULD NEVER BE USED!!"); }
  TDataLoop& operator=(const TDataLoop& other) { AbstractMethod("SHOULD NEVER BE USED!!"); }

  void ReadLoop();
  void Iteration();

#ifndef __CINT__
  std::thread read_thread;
  std::atomic_bool running;
  std::atomic_bool initialized;

  std::atomic_bool paused;
  std::condition_variable paused_wait;
  std::mutex pause_mutex;
#endif

  TRawFileIn* infile;
  RawDataQueue* queue;

  ClassDef(TDataLoop,0);
};

#endif /* _TDATALOOP_H_ */
