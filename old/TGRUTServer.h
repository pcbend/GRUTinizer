#ifndef _TGRUTSERVER_H_
#define _TGRUTSERVER_H_

#ifndef __CINT__
  #include <atomic>
  #include <memory>
  #include <thread>
#endif

#include <string>

#include "TList.h"
#include "TMonitor.h"
#include "TObject.h"
#include "TServerSocket.h"


class TGRUTServer : public TObject {
public:
  TGRUTServer(int port);
  virtual ~TGRUTServer();

  void Start();
  void Stop();

  int  GetPort()         { return port; }
  void SetPort(int new_port);

private:
  TGRUTServer(const TGRUTServer&) { MayNotUse(__PRETTY_FUNCTION__); }
  TGRUTServer& operator=(const TGRUTServer&) { MayNotUse(__PRETTY_FUNCTION__); }

  void Run();
  void OpenPort();
  void Iteration();
  void DoNewConnection(TServerSocket*);
  void DoRead(TSocket*);
  void DoWrite(TSocket*);

  TMonitor *monitor;
  TServerSocket *server;

  int port;
  TList readlist;
  TList writelist;

#ifndef __CINT__
  //std::unique_ptr<TServerSocket> server;
  std::atomic_bool is_running;
  std::chrono::milliseconds max_sleep;
  std::thread listen_thread;
#endif

  ClassDef(TGRUTServer,0);
};

#endif /* _TGRUTSERVER_H_ */
