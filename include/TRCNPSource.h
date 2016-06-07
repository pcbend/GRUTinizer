#ifndef _TRCNPSOURCE_H_
#define _TRCNPSOURCE_H_

#include <queue>
#include <cassert>
#include <fstream>
#include <sstream>
#include <queue>

#ifndef __CINT__
#include <thread>
#include <functional>
#endif

#include "TObject.h"
#include "TRawEvent.h"
#include "TRawSource.h"
#include "TChain.h"
#include "TFile.h"

#include "TRCNPEvent.h"
#include "RCNPEvent.h"
#include "ThreadsafeQueue.h"
#include "GRUTinizerInterface.h"

class TRCNPSource : public TRawEventSource {
public:

  TRCNPSource(const char* Command, kFileType file_type)
    : fCommand(Command), fFileType(file_type) {

    assert(file_type == kFileType::RCNP_BLD);

#ifndef __CINT__
    std::thread grloop(StartGRAnalyzer,Command,[&](RCNPEvent* event){
        gr_queue.Push(*event);
    });
#endif

    LoadFakeTimestamps();
  }
  TRCNPSource(TRCNPSource& source) { ; }

  ~TRCNPSource() {;}

  virtual std::string Status() const {
    return Form("%s: %s %8.2f MB given %s / %s %8.2f MB total %s  => %s %3.02f MB/s processed %s",
                SourceDescription().c_str(),
                DCYAN, GetBytesGiven()/1e6, RESET_COLOR,
                BLUE,  GetFileSize()/1e6, RESET_COLOR,
                GREEN, GetAverageRate()/1e6, RESET_COLOR);
  }
  virtual std::string SourceDescription() const {return "File: "+std::string("RCNP_BLD: ")+std::string(fCommand);}


  kFileType GetFileType() const { return fFileType; }
  long GetFileSize() const { return fFileSize; }

  virtual void Reset() {;}

protected:
  void SetFileSize(long file_size) { fFileSize = file_size; }
  void LoadFakeTimestamps() {

    std::string line; std::stringstream stream; ULong_t ts;
    ifstream file ("/projects/ceclub/sullivan/cagragr/GRUTinizer/timestamps.dat");
    if (file.is_open())
    {
      while ( getline (file,line) )
      {
	stream << line;
	stream >> ts;
	timestamps.push(ts);
	stream.str("");
	stream.clear();
	//std::cout << ts << std::endl;
      }
      file.close();
    }
  }

private:
  TRCNPSource() {;}
  virtual int GetEvent(TRawEvent& event) {
    TRCNPEvent rcnp_evt;
    rcnp_evt.event = new RCNPEvent;
    if (gr_queue.Pop(*rcnp_evt.event,0) == -1 ){
        return -1;
    }

    event = rcnp_evt;
    event.SetFileType(fFileType);

    // if there are no more events, signal termination
    //if (fCurrentEntry == fNumEvents) { return -1; }



    // set the timestamp of the ttree event
    if (timestamps.size()==0) {
      std::cout << "End of time stamps" << std::endl;
      return -1;
    }
    event.SetFragmentTimestamp(timestamps.front());
    timestamps.pop();

    // increment the event count
    //fCurrentEntry++;
    //fEvent = nullptr;
    return sizeof(*rcnp_evt.event);
  }

  const char* fCommand;
  kFileType fFileType;
  long fFileSize;
  ThreadsafeQueue<RCNPEvent> gr_queue;

  std::queue<Long_t> timestamps;

  ClassDef(TRCNPSource,0);
};

#endif
