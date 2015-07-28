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

class TRawFileIn;

class TDataLoop : public TNamed {
public:
  static TDataLoop* Instance();
  virtual ~TDataLoop();

  void ProcessFile(const char* filename, kFileType file_type = kFileType::UNKNOWN_FILETYPE);
  void ProcessRing(const char* filename);

  void Start();
  void Pause();
  void Resume();
  void Stop();
  bool IsPaused();
  void Join();

  virtual void Initialize() { }
  virtual void Finalize()   { }
  virtual int ProcessEvent(TRawEvent& event) { AbstractMethod("ProcessEvent()"); }


  template<typename T, typename... Params>
  static void CreateDataLoop(Params&&... params){
    if(data_loop){
      return;
    }

    data_loop = new T(std::forward<Params>(params)...);
  }

  const TRawFileIn* GetInfile() { return infile; }

protected:
  TDataLoop();


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


  TRawFileIn* infile;

  ClassDef(TDataLoop,0);
};

#endif /* _TDATALOOP_H_ */
