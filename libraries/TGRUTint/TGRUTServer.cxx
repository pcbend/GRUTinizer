#include "Globals.h"
#include "TGRUTServer.h"

#include <chrono>
#include <iostream>
#include <vector>

#include "TError.h"
#include "TMessage.h"
#include "TSocket.h"

#include "TGRUTint.h"

TGRUTServer::TGRUTServer(int port)
  : port(port), server(NULL), is_running(false),
    max_sleep(std::chrono::seconds(10)), monitor(NULL) { }

TGRUTServer::~TGRUTServer() {
  Stop();
  if(monitor) {
    monitor->RemoveAll();
    monitor->Delete();
  }
}

void TGRUTServer::Start() {
  if(port>0 && !is_running){
    is_running = true;
    listen_thread = std::thread(&TGRUTServer::Run, this);
  }
}

void TGRUTServer::Stop(){
  if(is_running){
    is_running = false;
    listen_thread.join();
  }
}

void TGRUTServer::Run() {
  OpenPort();

  while(is_running){
    Iteration();
  }
}

void TGRUTServer::SetPort(int new_port) {
  port = new_port;

  if(is_running){
    Stop();
    Start();
  }
}

void TGRUTServer::OpenPort(){
  while(is_running && port<65535) {
    auto old_val = gErrorIgnoreLevel;
    gErrorIgnoreLevel = kFatal;
    server = new TServerSocket(port,true);
    gErrorIgnoreLevel = old_val;

    if(server->IsValid()){
      monitor = new TMonitor(false);
      monitor->Add(server);
      server->SetOption(kNoBlock,1);
      printf(BLUE "now listening on port: %i" RESET_COLOR  "\n",port);
      return;
    }

    std::cerr << "Could not open port " << port << ", trying port " << port+1 << std::endl;
    port++;
  }

  std::cerr << "No open ports" << std::endl;
}

void TGRUTServer::Iteration(){
  int numsockets = monitor->Select(&readlist,&writelist,20);
  if(numsockets==0 || numsockets==-2 ) { //timeout
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return;
  } else {
    //do reading.
    TIter read(&readlist);
    TIter write(&writelist);
    while(TSocket *sock = (TSocket*)read.Next()) {
      if(sock->IsA() == TServerSocket::Class()) {
        DoNewConnection((TServerSocket*)sock);
      } else {
        DoRead(sock);
      }
    }
    while(TSocket *sock = (TSocket*)read.Next()) {
      DoWrite(sock);
    }
  }
}

void TGRUTServer::DoWrite(TSocket *sock) {
  printf(BLUE "%s called." RESET_COLOR "\n",__PRETTY_FUNCTION__);  fflush(stdout);
  if(!sock || (sock==(TSocket*)-1)) return;
  return;
}

void TGRUTServer::DoNewConnection(TServerSocket *sock) {
  if(!sock || (sock==(TSocket*)-1)) return;
  // Verify that the connection is from localhost
  TSocket *newsock = sock->Accept();
  std::string connection_location = newsock->GetInetAddress().GetHostAddress();
  if(connection_location != "127.0.0.1"){
    std::cerr << "Attempted connection from " << connection_location << "\n"
              << "Only localhost is allowed to connect." << std::endl;
    delete newsock;  //calls close on socket
    return;
  }
  monitor->Add(newsock);
  return;
}

void TGRUTServer::DoRead(TSocket *sock) {
  if(!sock || (sock==(TSocket*)-1)) return;
  TMessage *mess;
  sock->Recv(mess);
  switch(mess->What()) {
    case kMESS_STRING: {
      char str[256];
      mess->ReadString(str,256);
      TObject* obj = TGRUTint::instance()->DelayedProcessLine(str);
      if(obj){
        sock->SendObject(obj);
        delete obj;
      } else {
        sock->Send("no response given");
      }
      monitor->Remove(sock);
      }
      break;
    case kMESS_OBJECT: {


      }
      break;
    default:
      break;
  };
  delete mess;
  return;
}
