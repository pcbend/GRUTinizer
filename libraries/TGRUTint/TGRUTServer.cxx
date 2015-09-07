#include "Globals.h"
#include "TGRUTServer.h"

#include <chrono>
#include <iostream>
#include <vector>

#include "TError.h"
#include "TMessage.h"
#include "TSocket.h"

#include "TGRUTint.h"

//TGRUTServer::TGRUTServer()
//  : port(-1), server(NULL), is_running(false),
//    max_sleep(std::chrono::minutes(5)), monitor(NULL) { 
//}

TGRUTServer::TGRUTServer(int port)
  : port(port), server(NULL), is_running(false),
    max_sleep(std::chrono::seconds(10)), monitor(NULL) {
  //Start();
}

TGRUTServer::~TGRUTServer() {
  Stop();
  if(monitor) {
    monitor->RemoveAll();
    monitor->Delete();
  }
}

void TGRUTServer::Start() {
  //printf(BLUE "%s called." RESET_COLOR "\n",__PRETTY_FUNCTION__);  fflush(stdout);
  if(port>0 && !is_running){
    is_running = true;
    listen_thread = std::thread(&TGRUTServer::Run, this);
  }
}

void TGRUTServer::Stop(){
  //printf(BLUE "%s called." RESET_COLOR "\n",__PRETTY_FUNCTION__);  fflush(stdout);
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
  //printf(BLUE "%s called." RESET_COLOR "\n",__PRETTY_FUNCTION__);  fflush(stdout);
  //printf(BLUE "attemping new TSocketServer on %i." RESET_COLOR "\n",port);  fflush(stdout);
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
  //printf(BLUE "%s called." RESET_COLOR "\n",__PRETTY_FUNCTION__);  fflush(stdout);
  int numsockets = monitor->Select(&readlist,&writelist,20);
  if(numsockets==0 || numsockets==-2 ) { //timeout
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  
    return;
//  } else if(numsockets<0) { //error state.
//    fprintf(stderr,"%s error[%i]; stopping server.\n",__PRETTY_FUNCTION__,numsockets);
//    Stop();
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
  //printf(BLUE "%s called." RESET_COLOR "\n",__PRETTY_FUNCTION__);  fflush(stdout);
  if(!sock || (sock==(TSocket*)-1)) return;
  // Verify that the connection is from localhost
  std::string connection_location = sock->GetInetAddress().GetHostAddress();
  TSocket *newsock = sock->Accept();
  monitor->Add(newsock);
//  if(connection_location != "127.0.0.1"){
//    std::cerr << "Attempted connection from " << connection_location << "\n"
//      	<< "Only localhost is allowed to connect." << std::endl;
    //readlist.Remove(sock);
//    delete sock;  //calls close on socket
//    return;
//  }
  //monitor->Add(sock);
  //std::cout << "Just accepted a connection" << std::endl;
  return;
}
  
void TGRUTServer::DoRead(TSocket *sock) {
  printf(BLUE "%s called." RESET_COLOR "\n",__PRETTY_FUNCTION__);  fflush(stdout);
  if(!sock || (sock==(TSocket*)-1)) return;
  TMessage *mess;
  sock->Recv(mess);
  switch(mess->What()) {
    case kMESS_STRING: {
      char str[256];
      mess->ReadString(str,256);
      //printf("Client %i: %s\n",sock==server ? 0:1,str);
      TGRUTint::instance()->DelayedProcessLine(str);
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

//  if(sock && (sock != (TSocket*)-1)){
//    std::cout << "Just accepted a connection" << std::endl;
/*

    // Verify that the connection is from localhost
    std::string connection_location = sock->GetInetAddress().GetHostAddress();
    if(connection_location == "127.0.0.1"){
      //monitor.Add(sock);
    } else {
      std::cerr << "Attempted connection from " << connection_location << "\n"
		<< "Only localhost is allowed to connect." << std::endl;
      delete sock;
      return;
    }
  } else { 
    return;
  }

  //sock = monitor.Select(10);
  //if(sock && (long(sock) != -1)){
    std::cout << "TMonitor gave me " << sock << std::endl;
    TMessage* message;
    int bytes = sock->Recv(message);
    std::cout << "Received " << bytes << " bytes" << std::endl;
    if(bytes == -4){
      // Timeout, catch it next time
    } else if (bytes <=0 ) {
      //monitor.Remove(sock);
    } else {
      std::cout << "We have a message" << std::endl;
      TObject* obj = message->ReadObject(message->GetClass());
      std::cout << "Object name: " << obj->GetName() << std::endl;
      if(obj->Class() == TString::Class()){
  	std::cout << "We have a string" << std::endl;
  	TString* string = (TString*)obj;
  	TGRUTint::instance()->DelayedProcessLine(string->Data());
      }
      delete obj;
      delete message;
    }
  //}
*/
//}
