#include "TGRUTServer.h"

#include <chrono>
#include <iostream>
#include <vector>

#include "TError.h"
#include "TApplication.h"
#include "TSocket.h"

TGRUTServer::TGRUTServer()
  : port(-1), server(nullptr), is_running(false),
    max_sleep(std::chrono::seconds(10)) { }

TGRUTServer::TGRUTServer(int port)
  : port(port), server(nullptr), is_running(false),
    max_sleep(std::chrono::seconds(10)) {
  Start();
}

TGRUTServer::~TGRUTServer() {
  Stop();
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
    bool res = ReadCommand();
    if(!res){
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
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
  while(is_running && port<65535){
    auto old_val = gErrorIgnoreLevel;
    gErrorIgnoreLevel = kFatal;
    server = std::unique_ptr<TServerSocket>(new TServerSocket(port));
    gErrorIgnoreLevel = old_val;

    if(server->IsValid()){
      std::cerr << "Listening on port " << port << std::endl;
      server->SetOption(kNoBlock,1);
      return;
    }

    std::cerr << "Could not open port " << port << ", trying port " << port+1 << std::endl;
    port++;
  }

  std::cerr << "No open ports" << std::endl;
}

std::string TGRUTServer::ReadFromSocket(TSocket& socket){
  socket.SetOption(kNoBlock,1);
  std::chrono::milliseconds time_slept(0);

  std::vector<char> buf;
  buf.reserve(128);
  char c = 1;
  while (c && time_slept < max_sleep) {
    int err = socket.RecvRaw(&c, 1);
    if(err == -4) {
      time_slept += std::chrono::milliseconds(10);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } else {
      buf.push_back(c);
    }
  }

  return std::string(buf.begin(), buf.end());
}

bool TGRUTServer::ReadCommand(){
  if(!server->IsValid()){
    return false;
  }

  // Who uses -1 cast to a pointer to indicate an error?
  auto rawp_socket = server->Accept();
  if(!rawp_socket || long(rawp_socket) == -1){
    return false;
  }
  auto socket = std::unique_ptr<TSocket>(rawp_socket);

  // Verify that the connection is from localhost
  std::string connection_location = socket->GetInetAddress().GetHostAddress();
  if(connection_location != "127.0.0.1"){
    std::cerr << "Attempted connection from " << connection_location << "\n"
              << "Only localhost is allowed to connect." << std::endl;
    return false;
  }

  std::string message = ReadFromSocket(*socket);

  // Do something with the command.
  std::cout << "\n" << "Received command \"" << message << "\"" << std::endl;
  gApplication->ProcessLine(message.c_str());

  return true;
}
