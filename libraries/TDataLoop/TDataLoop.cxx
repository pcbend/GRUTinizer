#include "TDataLoop.h"

#include <chrono>
#include <thread>
#include <utility>

#include "TRawSource.h"
#include "TString.h"

std::map<std::string,TDataLoop*> TDataLoop::fdataloopmap;

TDataLoop::TDataLoop(std::string name,TRawEventSource* source) 
  : source(source) { 
  if(!name.length())
    name = Form("dataloop%i",GetNDataLoops());
  fname = name;
  fdataloopmap.insert(std::make_pair(fname,this));
}

TDataLoop::~TDataLoop(){
  delete source; // do we really want the loop to take ownership?
}

int TDataLoop::GetNDataLoops() { 
  return fdataloopmap.size();
}

TDataLoop *TDataLoop::Get(std::string name,TRawEventSource* source) { 
  if(fdataloopmap.count(name))
    return fdataloopmap.at(name);
  else if(source)
    return new TDataLoop(name,source);
  else
    return 0;
}


bool TDataLoop::Iteration() {
  TRawEvent evt;
  int bytes_read = source->Read(evt);

  if(bytes_read < 0){
    // Error, and no point in trying again.
    return false;
  } else if(bytes_read > 0){
    // A good event was returned
    output_queue.Push(evt);
    return true;
  } else {
    // Nothing returned this time, but I might get something next time.
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return true;
  }
}


std::string TDataLoop::Status() {
  return source->Status();
}
