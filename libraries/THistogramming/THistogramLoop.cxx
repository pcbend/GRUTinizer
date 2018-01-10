#include "THistogramLoop.h"

#include "TFile.h"

#include "TGRUTint.h"
#include "TGRUTOptions.h"
#include "TPreserveGDirectory.h"
#include "GValue.h"
#include "TChannel.h"

THistogramLoop * THistogramLoop::Get(std::string name) {
  if(name.length()==0)
    name = "histo_loop";
  THistogramLoop *loop = dynamic_cast<THistogramLoop*>(StoppableThread::Get(name));
  if(!loop)
    loop = new THistogramLoop(name);
  return loop;
}

THistogramLoop::THistogramLoop(std::string name)
  : StoppableThread(name),
    output_file(0), output_filename("last.root"),
    input_queue(std::make_shared<ThreadsafeQueue<TUnpackedEvent*> >()),
    output_queue(std::make_shared<ThreadsafeQueue<TUnpackedEvent*> >()) {
  LoadLib(TGRUTOptions::Get()->CompiledHistogramFile());
}

THistogramLoop::~THistogramLoop() {
  CloseFile();
}

void THistogramLoop::ClearQueue() {
  while(input_queue->Size()){
    TUnpackedEvent* event = NULL;
    input_queue->Pop(event);
    if(event){
      delete event;
    }
  }

  while(output_queue->Size()){
    TUnpackedEvent* event = NULL;
    output_queue->Pop(event);
    if(event){
      delete event;
    }
  }
}

bool THistogramLoop::Iteration() {
  TUnpackedEvent* event = NULL;
  input_queue->Pop(event);

  if(event) {
    if(!output_file){
      OpenFile();
    }

    compiled_histograms.Fill(*event);
    output_queue->Push(event);
    return true;

  } else if(input_queue->IsFinished()) {
    output_queue->SetFinished();
    return false;

  } else {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return true;
  }
}

void THistogramLoop::ClearHistograms() {
  compiled_histograms.ClearHistograms();
}

void THistogramLoop::OpenFile() {
  TPreserveGDirectory preserve;
  output_file = TGRUTint::instance()->OpenRootFile(output_filename,
                                                   "RECREATEONLINE");
  compiled_histograms.SetDefaultDirectory(output_file);
}

void THistogramLoop::CloseFile() {
  Write();

  if(output_file){
    output_file->Close();
    output_file = 0;
    output_filename = "last.root";
  }
}

void THistogramLoop::Write() {
  if(GetOutputFilename() == "/dev/null") {
    return;
  }

  TPreserveGDirectory preserve;
  if(output_file){
    output_file->cd();
    compiled_histograms.Write();
    if(GValue::Size()) {
      GValue::Get()->Write();
      printf(BLUE "\t%i GValues written to file %s" RESET_COLOR "\n",GValue::Size(),gDirectory->GetName());
    }
    if(TChannel::Size()) {
      TChannel::Get()->Write();
      printf(BLUE "\t%i TChannels written to file %s" RESET_COLOR "\n",TChannel::Size(),gDirectory->GetName());
    }
  }
}

void THistogramLoop::LoadLibrary(std::string library) {
  compiled_histograms.Load(library);
}

std::string THistogramLoop::GetLibraryName() const {
  return compiled_histograms.GetLibraryName();
}

TList* THistogramLoop::GetObjects() {
  return compiled_histograms.GetObjects();
}

TList* THistogramLoop::GetGates() {
  return compiled_histograms.GetGates();
}

void THistogramLoop::SetOutputFilename(const std::string& name){
  output_filename = name;
}

std::string THistogramLoop::GetOutputFilename() const {
  return output_filename;
}

void THistogramLoop::AddCutFile(TFile* cut_file) {
  compiled_histograms.AddCutFile(cut_file);
}
