#include "TRCNPSource.h"
#include <atomic>

extern std::atomic<int> stop_rcnp_signal;

#ifdef RCNP
#include "GRUTinizerInterface.h"


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
                       TGRUTOptions::Get()->SaveRCNPTree(),
                       !TGRUTOptions::Get()->StartGUI()
    );

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

  double time = 0;
  event.SetDataPtr((void*)rcnp);
  if (TGRUTOptions::Get()->GRSingles()) {
    // singles (ignoring myriad timestamp)
    static ULong_t counter = 0;
    counter += TGRUTOptions::Get()->BuildWindow()*1.5;
    time = counter;
  }
  else {
    // normal, use the GR myriad timestamp and if it's not present set the time to a random constant
    time = rcnp->GR_MYRIAD(0);
    if (time == -441441) {

      static int not_found = 0;
      if (not_found < 100) {
        std::cout << "GR Myriad timestamp not found!!!" << std::endl;
        if (not_found == 99) {
          std::cout << "More than 100 GR events are missing a timestamp. This warning is being supressed, but you should probably investigate this." << std::endl;
        }
      }

      time = 2112;
    }
  }

  rcnp->SetTimestamp(time);
  event.SetFragmentTimestamp(time);

  return sizeof(rcnp);
}

std::string TRCNPSource::Status(bool long_description) const {
  return Form("%s: %s %8.2f MB given %s / %s  Unknown MB total %s  => %s %3.02f MB/s processed %s",
              SourceDescription(long_description).c_str(),
              DCYAN, GetBytesGiven()/1e6, RESET_COLOR,
              BLUE, RESET_COLOR,
              GREEN, GetAverageRate()/1e6, RESET_COLOR);
}
std::string TRCNPSource::SourceDescription(bool long_description) const {return "File: "+std::string("RCNP_BLD: ")+fCommand;}


// template<>
// int ThreadsafeQueue<RCNPEvent>::ObjectSize(RCNPEvent& event) {
//   return event.data.size();
// }

template<>
int ThreadsafeQueue<RCNPEvent*>::ObjectSize(RCNPEvent*& event) const {
  return event->data.size();
}


#endif
