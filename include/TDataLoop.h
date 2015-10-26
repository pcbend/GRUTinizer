#ifndef _TDATALOOP_H_
#define _TDATALOOP_H_

#ifndef __CINT__
#include <atomic>
#include <condition_variable>
#include <thread>
#endif

#include <cstdlib>
#include <iostream>

#include "TNamed.h"

#include "TRawEvent.h"

class TRawEventSource;

class TDataLoop : public TNamed {
public:
  static TDataLoop* Instance();
  static void DeleteInstance();

  virtual ~TDataLoop();

  void ProcessFile(const char* filename, bool is_online = false);
  void ProcessFile(const std::vector<std::string>& filenames, bool is_online = false);
  void ProcessRing(const char* filename);

  void Start();
  void Pause();
  void Resume();
  void Stop();
  bool IsPaused();
  void Join();

  virtual bool Initialize() { return true; }
  virtual void Finalize()   { }
  virtual int ProcessEvent(TRawEvent& event) { AbstractMethod("ProcessEvent()"); }

  bool IsRunning() { return running; }


  template<typename T, typename... Params>
  static void CreateDataLoop(Params&&... params){
    if(data_loop){
      return;
    }

    data_loop = new T(std::forward<Params>(params)...);
  }

  const TRawEventSource* GetInfile() { return infile; }

protected:
  TDataLoop();
  virtual bool CanStart() { return true; }

  TRawEventSource* infile;

#ifndef __CINT__
  std::thread read_thread;
  std::atomic_bool running;
  std::atomic_bool initialized;

  std::atomic_bool paused;
  std::condition_variable paused_wait;
  std::mutex pause_mutex;
#endif

private:
  static TDataLoop* data_loop;

  void ReadLoop();
  void Iteration();

  void ProcessSource();


  TDataLoop(const TDataLoop& other) { MayNotUse("TDataLoop()"); }
  TDataLoop& operator=(const TDataLoop& other) { MayNotUse("TDataLoop::operator="); }


  ClassDef(TDataLoop,0);
};

#endif /* _TDATALOOP_H_ */
