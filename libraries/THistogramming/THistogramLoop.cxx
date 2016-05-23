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
  : StoppableThread(name),
    output_file(0), previous_dir(0), output_filename("last.root"),
    stopsent(false) {
  LoadLib(TGRUTOptions::Get()->CompiledHistogramFile());
}

THistogramLoop::~THistogramLoop() {
  CloseFile();
}

int THistogramLoop::Push(TUnpackedEvent *event) {
  return input_queue.Push(event);
}

void THistogramLoop::ClearQueue() {
  while(input_queue.Size()){
    TUnpackedEvent* event = NULL;
    input_queue.Pop(event);
    if(event){
      delete event;
    }
  }
}

bool THistogramLoop::Iteration() {
  TUnpackedEvent* event = NULL;
  input_queue.Pop(event);
  if(event) {
    if(!output_file){
      OpenFile();
    }


    compiled_histograms.Fill(*event);

    delete event;
  }
  if(stopsent && input_queue.Size()<1) {
    return false;
  }

  return true;
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
  /*
  if(output_file){
    {
      TPreserveGDirectory preserve;
      output_file->cd();
      compiled_histograms.Write();
    }

    compiled_histograms.SetDefaultDirectory(NULL);
  */
  Write();

  if(output_file){
    output_file->Close();
    output_file = 0;
    output_filename = "last.root";
  }
}

void THistogramLoop::Write() {
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

// void THistogramLoop::SetReplaceVariable(const char* name, double value) {
//   compiled_histograms.SetReplaceVariable(name, value);
// }

// void THistogramLoop::RemoveVariable(const char* name) {
//   compiled_histograms.RemoveVariable(name);
// }

// TList* THistogramLoop::GetVariables() {
//   return compiled_histograms.GetVariables();
// }

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

void THistogramLoop::cd(Option_t* opt) {
  if(strncmp(opt,"..",2)){
    popd();
  } else {
    if(output_file){
      previous_dir = gDirectory;
      output_file->cd();
    }
  }
}

void THistogramLoop::popd() {
  if(previous_dir){
    previous_dir->cd();
    previous_dir = 0;
  }
}
