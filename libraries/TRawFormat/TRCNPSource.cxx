#include "TRCNPSource.h"
#include "GRUTinizerInterface.h"

extern atomic<int> stop_rcnp_signal;

TRCNPSource::TRCNPSource(const char* Command, kFileType file_type)
  : fCommand(Command), fFileType(file_type) {

  assert(file_type == kFileType::RCNP_BLD);

  fFuture = std::async(std::launch::async,
                       StartGRAnalyzer,
                       Command,
                       &stop_rcnp_signal,
                       [&](RCNPEvent* event) {
                         rcnp_queue.Push(event);
                       },
                       TGRUTOptions::Get()->SaveRCNPTree());

  //LoadFakeTimestamps();
  std::this_thread::sleep_for(std::chrono::seconds(4));
}

int TRCNPSource::GetEvent(TRawEvent& event) {
  event.SetFileType(fFileType);

  // TRCNPEvent* rcnp_evt = new TRCNPEvent;
  RCNPEvent* rcnp;// = new RCNPEvent;

  // Try to get event from RCNP event queue
  if (rcnp_queue.Pop(rcnp,200) == -1 ){
#ifndef __CINT__
    // if offline
    if (TGRUTOptions::Get()->ExitAfterSorting()) {
      auto status = fFuture.wait_for(std::chrono::milliseconds(0));
      // check if RCNP analyzer is done
      if (status == std::future_status::ready) {
        std::cout << "RCNP analyzer thread finished ##" << std::endl;
        return -1;
      } else {
        std::cout << "RCNP analyzer still running, awaiting data.." << std::endl;
        // try again to pop an event
        if (rcnp_queue.Pop(rcnp,1000) == -1) {
          // if it failed again check if the RCNP analyzer is done now
          status = fFuture.wait_for(std::chrono::milliseconds(1000));
          if (status == std::future_status::ready) {
            return -1;
          } else {
            throw std::runtime_error("TRCNPSource::GetEvent GRAnalyzer loop is hung.");
          }
        }
      }
    } else {
      return -1;
    }
#endif
  }


  //char* ptrbytes = (char*)calloc(1,sizeof(rcnp));
  //*reinterpret_cast<RCNPEvent**>(ptrbytes) = rcnp;
  //TSmartBuffer eventbuffer(ptrbytes,sizeof(rcnp));
  //event.SetData(eventbuffer);

  event.SetDataPtr((void*)rcnp);
  double time = rcnp->GR_MYRIAD(0);
  if (time == -441441) { time = 11; }

  rcnp->SetTimestamp(time);
  event.SetFragmentTimestamp(time);
  return sizeof(rcnp);
}

std::string TRCNPSource::Status() const {
  return Form("%s: %s %8.2f MB given %s / %s  Unknown MB total %s  => %s %3.02f MB/s processed %s",
              SourceDescription().c_str(),
              DCYAN, GetBytesGiven()/1e6, RESET_COLOR,
              BLUE, RESET_COLOR,
              GREEN, GetAverageRate()/1e6, RESET_COLOR);
}
std::string TRCNPSource::SourceDescription() const {return "File: "+std::string("RCNP_BLD: ")+fCommand;}


// template<>
// int ThreadsafeQueue<RCNPEvent>::ObjectSize(RCNPEvent& event) {
//   return event.data.size();
// }

template<>
int ThreadsafeQueue<RCNPEvent*>::ObjectSize(RCNPEvent*& event) {
  return event->data.size();
}
