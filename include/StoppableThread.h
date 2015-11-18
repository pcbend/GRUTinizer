#ifndef _STOPPABLETHREAD_H_
#define _STOPPABLETHREAD_H_

#ifndef __CINT__
#include <atomic>
#include <condition_variable>
#include <thread>
#endif

#include <string>
#include <map>

#include "TObject.h"

class StoppableThread {
  public:
    StoppableThread(std::string name);
    static StopppableThread *Get(std::string name);
    virtual ~StoppableThread();

    void Resume();
    void Pause();
    void Stop();
    bool IsPaused();
    bool IsRunning();
    void Join();

    virtual std::string Status() { return ""; }
    std::string Name() const     { return fname; }

    //protected:
    virtual bool Iteration() = 0;

  protected:
    static std::map<std::string,StoppableThread*> fthreadmap;
    static int GetNThreads();

  private:
    StoppableThread(const StoppableThread& other) { }
    StoppableThread& operator=(const StoppableThread& other) { }

    std::string fname;

    void Loop();

#ifndef __CINT__
    std::thread thread;
    std::atomic_bool running;
    std::atomic_bool force_stop;
    std::atomic_bool paused;
    std::condition_variable paused_wait;
    std::mutex pause_mutex;
#endif

    ClassDef(StoppableThread, 0);
};


#endif /* _STOPPABLETHREAD_H_ */
