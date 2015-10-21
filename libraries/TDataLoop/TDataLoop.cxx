#include "TDataLoop.h"

#include "RawDataQueue.h"
#include "TGRUTOptions.h"
#include "TMultiRawFile.h"
#include "TOrderedRawFile.h"
#include "TRawEventSource.h"

TDataLoop* TDataLoop::data_loop = NULL;

TDataLoop* TDataLoop::Instance() {
  if(!data_loop){
    std::cerr << "TDataLoop::Get() called before initializated" << std::endl;
    exit(1);
  }
  return data_loop;
}

void TDataLoop::DeleteInstance() {
  if(data_loop){
    delete data_loop;
    data_loop = NULL;
  }
}

TDataLoop::TDataLoop()
  : running(false), initialized(false), paused(false),
    infile(NULL) {
}

TDataLoop::~TDataLoop(){
  Stop();
}

void TDataLoop::Start(){
  if(!CanStart()){
    return;
  }
  if(running && !initialized){
    std::cout << "Initializing" << std::endl;
    bool success = Initialize();
    if(success){
      initialized = true;
      Resume();
    }
  } else if (running && paused) {
    std::cout << "Resuming" << std::endl;
    Resume();
  } else {
    std::cout << "Ignoring the start signal" << std::endl;
  }
}

void TDataLoop::ProcessFile(const char* filename, bool is_online){
  if(running || infile){
    std::cerr << "Data loop already running" << std::endl;
  }

  TRawEventSource* infile = TRawEventSource::EventSource(filename, is_online);
  if(TGRUTOptions::Get()->TimeSortInput()){
    infile = new TOrderedRawFile(infile);
  }

  if(infile->GetLastErrno() != 0)  {
    std::cerr << "Error opening file " << filename << ": " << infile->GetLastError()
              << " (Errno=" << infile->GetLastErrno() << ")" << std::endl;
    delete infile;
    return;
  }

  this->infile = infile;
  ProcessSource();
}

void TDataLoop::ProcessFile(const std::vector<std::string>& filenames, bool is_online){
  if(running || infile){
    std::cerr << "Data loop already running" << std::endl;
  }

  TMultiRawFile* infile = new TMultiRawFile;

  for(auto& filename : filenames){
    TRawEventSource* infile_segment = TRawEventSource::EventSource(filename.c_str(), is_online);
    if(TGRUTOptions::Get()->TimeSortInput()){
      infile_segment = new TOrderedRawFile(infile_segment);
    }
    infile->AddFile(infile_segment);
  }

  if(infile->IsValid()){
    this->infile = infile;
    ProcessSource();
  } else {
    delete infile;
  }
}

void TDataLoop::ProcessRing(const char* filename){
  if(running || infile){
    std::cerr << "Data loop already running" << std::endl;
  }

  TRawEventSource* infile = TRawEventSource::EventSource(filename, true, true);
  if(TGRUTOptions::Get()->TimeSortInput()){
    infile = new TOrderedRawFile(infile);
  }

  if(infile->GetLastErrno() != 0)  {
    std::cerr << "Error opening file " << filename << ": " << infile->GetLastError()
              << " (Errno=" << infile->GetLastErrno() << ")" << std::endl;
    delete infile;
    return;
  }

  this->infile = infile;
  ProcessSource();
}


void TDataLoop::ProcessSource() {
  if(!infile){
    return;
  }

  if(infile->GetLastErrno()) {
    std::cout << "Problem opening file: << " << infile->SourceDescription() << std::endl;
    std::cout << "\tFile State: " << infile->GetLastError() << std::endl;
  }
  running = true;
  Pause();
  read_thread = std::thread(&TDataLoop::ReadLoop, this);
}

void TDataLoop::Pause() {
  paused = true;
}

bool TDataLoop::IsPaused() {
  return paused;
}

void TDataLoop::Resume() {
  paused = false;
  paused_wait.notify_all();
}

void TDataLoop::Stop() {
  if(running) {
    Resume();
    running = false;

    // If this is not running from the read thread, wait for it to close.
    if(std::this_thread::get_id() != read_thread.get_id() &&
       read_thread.joinable()){
      read_thread.join();
    }

    Finalize();
    initialized = false;
  }
}


void TDataLoop::ReadLoop() {
  std::cout << "Read loop starting" << std::endl;

  int i=0;
  while(running && i<10000){
    i++;
    {
      std::unique_lock<std::mutex> lock(pause_mutex);
      while(paused){
        paused_wait.wait(lock);
      }
    }
    Iteration();
  }
  std::cout << "End of read loop" << std::endl;
  delete infile;
  infile = NULL;
}

void TDataLoop::Iteration() {
  TRawEvent evt;
  int bytes_read = infile->Read(evt);
  if(bytes_read > 0){
    ProcessEvent(evt);
  } else {
    Stop();
  }
}

void TDataLoop::Join() {
  if(read_thread.joinable()){
    read_thread.join();
  }
}
