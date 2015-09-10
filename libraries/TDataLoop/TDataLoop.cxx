#include "TDataLoop.h"

#include "RawDataQueue.h"
#include "TGRUTOptions.h"
#include "TMultiRawFile.h"
#include "TRawFile.h"

TDataLoop* TDataLoop::data_loop = NULL;

TDataLoop* TDataLoop::Instance() {
  if(!data_loop){
    std::cerr << "TDataLoop::Get() called before initializated" << std::endl;
    exit(1);
  }
  return data_loop;
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

void TDataLoop::ProcessFile(const char* filename, kFileType file_type){
  if(running || infile){
    std::cerr << "Data loop already running" << std::endl;
  }

  if(file_type == kFileType::UNKNOWN_FILETYPE){
    file_type = TGRUTOptions::Get()->DetermineFileType(filename);
  }

  switch(file_type){
  case kFileType::NSCL_EVT:
  case kFileType::GRETINA_MODE2:
  case kFileType::GRETINA_MODE3:
    break;
  default:
    std::cerr << "Attempting to process an unknown file type on file " << filename << std::endl;
  }

  TRawFileIn* infile = new TRawFileIn(filename, file_type);

  if(infile->GetLastErrno() != 0)  {
    std::cerr << "Error opening file " << filename << ": " << infile->GetLastError()
              << " (Errno=" << infile->GetLastErrno() << ")" << std::endl;
    return;
  }

  this->infile = infile;
  ProcessSource();
}

void TDataLoop::ProcessFile(const std::vector<std::string>& filenames){
  if(running || infile){
    std::cerr << "Data loop already running" << std::endl;
  }

  TMultiRawFile* infile = new TMultiRawFile;

  for(auto& filename : filenames){
    infile->AddFile(filename.c_str());
  }

  if(infile->IsValid()){
    this->infile = infile;
    ProcessSource();
  } else {
    delete infile;
  }
}

void TDataLoop::ProcessRing(const char* filename){
  std::cerr << "ProcessRing not implemented yet" << std::endl;
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
    if(std::this_thread::get_id() != read_thread.get_id()){
      read_thread.join();
    }

    Finalize();
    initialized = false;
  }
}


void TDataLoop::ReadLoop() {
  std::cout << "Read loop starting" << std::endl;

  while(running){
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
  int bytes_read = infile->Read(&evt);
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
