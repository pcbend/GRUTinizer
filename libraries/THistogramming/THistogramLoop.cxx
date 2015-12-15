#include "THistogramLoop.h"

#include "TPreserveGDirectory.h"


THistogramLoop * THistogramLoop::Get(std::string name) {
  if(name.length()==0)
    name = "histo_loop";
  THistogramLoop *loop = dynamic_cast<THistogramLoop*>(StoppableThread::Get(name));
  if(!loop)
    loop = new THistogramLoop(name);
  return loop;
}

/*
THistogramLoop::THistogramLoop(ThreadsafeQueue<TUnpackedEvent*>& input_queue,
                               ThreadsafeQueue<TUnpackedEvent*>& output_queue,
                               TDirectory* dir)
  : input_queue(input_queue), output_queue(output_queue),
    output_dir(dir) {
  compiled_histograms.SetDefaultDirectory(dir);
}
*/
THistogramLoop::THistogramLoop(std::string name) 
  : StoppableThread(name), stopsent(false) {
  std::string libname = getenv("GRUTSYS");
  libname.append("libraries/libMakeHistos.so");
  LoadLib(libname);
}

int THistogramLoop::Push(TUnpackedEvent *event) { 
  return input_queue.Push(event);
}


bool THistogramLoop::Iteration() {
  TUnpackedEvent* event = NULL;
  int error = input_queue.Pop(event);
  //if (error==0){
  //  return true;
  //}
  if(event) {
    compiled_histograms.Fill(*event);
    delete event;
    //output_queue.Push(event);
  } 
  if(stopsent && input_queue.Size()<1) {
    return false;
  }
  return true;
}

void THistogramLoop::ClearHistograms() {
  compiled_histograms.ClearHistograms();
}

void THistogramLoop::Write() {
  TPreserveGDirectory preserve;
  if(output_dir){
    output_dir->cd();
  }
  compiled_histograms.Write();
}

void THistogramLoop::LoadLibrary(std::string library) {
  compiled_histograms.Load(library);
}

std::string THistogramLoop::GetLibraryName() const {
  return compiled_histograms.GetLibraryName();
}

void THistogramLoop::SetReplaceVariable(const char* name, double value) {
  compiled_histograms.SetReplaceVariable(name, value);
}

void THistogramLoop::RemoveVariable(const char* name) {
  compiled_histograms.RemoveVariable(name);
}

TList* THistogramLoop::GetVariables() {
  return compiled_histograms.GetVariables();
}
