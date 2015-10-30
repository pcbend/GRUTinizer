#ifndef _STOPPABLETHREAD_H_
#define _STOPPABLETHREAD_H_

#ifndef __CINT__
#include <atomic>
#include <condition_variable>
#include <thread>
#endif

class StoppableThread {
public:
  StoppableThread();
  virtual ~StoppableThread();

#ifndef __CINT__
  StoppableThread(const StoppableThread& other) = delete;
  StoppableThread& operator=(const StoppableThread& other) = delete;
#endif

  void Start();
  void Pause();
  void Stop();
  bool IsPaused();
  bool IsRunning();
  void Join();

protected:
  virtual bool Iteration() = 0;

private:
  void Loop();

#ifndef __CINT__
  std::thread thread;
  std::atomic_bool running;
  std::atomic_bool force_stop;
  std::atomic_bool paused;
  std::condition_variable paused_wait;
  std::mutex pause_mutex;
#endif
};


#endif /* _STOPPABLETHREAD_H_ */
