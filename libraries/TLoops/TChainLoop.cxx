#include "TChainLoop.h"

#include <chrono>
#include <thread>

#include "TClass.h"
#include "TFile.h"
#include "TThread.h"

#include "TDetector.h"
#include "TGRUTint.h"
#include "TUnpackedEvent.h"

TChainLoop* TChainLoop::Get(std::string name,TChain *chain){
  if(name.length()==0){
    name = "chain_loop";
  }

  StoppableThread* thread = StoppableThread::Get(name);
  if(!thread){
    if(!chain && !gChain){
      return 0;
    } else if(!chain) {
      chain = gChain;
    }
    thread = new TChainLoop(name,chain);
  }
  return dynamic_cast<TChainLoop*>(thread);
}

TChainLoop::TChainLoop(std::string name, TChain *chain)
  : StoppableThread(name),
    fEntriesRead(0), fEntriesTotal(chain->GetEntries()),
    input_chain(chain),
    output_queue(std::make_shared<ThreadsafeQueue<TUnpackedEvent*> >()),
    fSelfStopping(true) {
  SetupChain();
}

TChainLoop::~TChainLoop() { }

void TChainLoop::ClearQueue() {
  while(output_queue->Size()){
    TUnpackedEvent* event = NULL;
    output_queue->Pop(event);
    if(event){
      delete event;
    }
  }
}

int TChainLoop::SetupChain() {
  if(!input_chain)
    return 0;

  TObjArray *array = input_chain->GetListOfBranches();
  for(int x=0;x<array->GetSize();x++) {
    TBranch *b = (TBranch*)array->At(x);
    if(b) {
      TClass *c = TClass::GetClass(b->GetName());
      if(c) {
        printf("Found  %s!\n",b->GetName());
        TDetector** det = new TDetector*;
	*det = NULL;
        det_map[c] = det;
        input_chain->SetBranchAddress(b->GetName(),det_map[c]);
      }
    }
  }
  return 0;
}

std::string TChainLoop::Status() {
  return Form("Event: %ld / %ld", long(fEntriesRead), fEntriesTotal);
}


void TChainLoop::Restart() {
  fEntriesRead = 0;
}

void TChainLoop::OnEnd() {
  output_queue->SetFinished();
}

bool TChainLoop::Iteration() {
  if(fEntriesRead >= fEntriesTotal){
    if(fSelfStopping) {
      return false;
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      return true;
    }
  }

  for(auto& elem : det_map){
    *elem.second = (TDetector*)elem.first->New();
  }
  input_chain->GetEntry(fEntriesRead++);

  TUnpackedEvent* event = new TUnpackedEvent;
  for(auto& elem : det_map){
    TDetector* det = *elem.second;
    if(!det->TestBit(TDetector::kUnbuilt)){
      event->AddDetector(det);
    } else {
      if(det->Timestamp()!=-1 && det->Size()!=0){
	std::cout << det->IsA()->GetName() << " was not present in this event (TS="
		  << det->Timestamp() << ")"
		  << std::endl;
      }
      delete det;
    }
  }

  output_queue->Push(event);
  return true;
}
