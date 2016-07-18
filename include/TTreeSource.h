#ifdef RCNP
#ifndef _TTREESOURCE_H_
#define _TTREESOURCE_H_

#include <queue>
#include <cassert>
#include <fstream>
#include <sstream>

#include "TObject.h"
#include "TRawEvent.h"
#include "TRawSource.h"
#include "TChain.h"
#include "TFile.h"

#include "RCNPEvent.h"

templateClassImp(TTreeSource)

template <typename T>
class TTreeSource : public TRawEventSource {
public:

  TTreeSource(const char* filename, const char* treename, const char* eventclassname, kFileType file_type)
    : fChain(treename), fEvent(0), fCurrentEntry(0) {

    assert(file_type == kFileType::ROOT_DATA);

    fFileType = file_type;

    fChain.Add(filename);

    fNumEvents = fChain.GetEntries();

    fFileSize =  fNumEvents*sizeof(T*);

    fChain.SetBranchAddress(eventclassname, &fEvent);

    LoadFakeTimestamps();
  }

  ~TTreeSource() {;}

  virtual std::string Status() const {
    return Form("%s: %s %8.2f MB given %s / %s %8.2f MB total %s  => %s %3.02f MB/s processed %s",
                SourceDescription().c_str(),
                DCYAN, GetBytesGiven()/1e6, RESET_COLOR,
                BLUE,  GetFileSize()/1e6, RESET_COLOR,
                GREEN, GetAverageRate()/1e6, RESET_COLOR);
  }
  virtual std::string SourceDescription() const {return "File: "+std::string(fChain.GetCurrentFile()->GetName());}


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
  TTreeSource() {;}
  virtual int GetEvent(TRawEvent& event) {
    event.SetFileType(fFileType);

    // if there are no more events, signal termination
    if (fCurrentEntry == fNumEvents) { return -1; }
    // copy construct a new object from the old
    fEvent = new T();
    // fill the new object with current event data
    fChain.GetEntry(fCurrentEntry);
    // create a small memory buffer to hold the pointer to the current entry
    char* ptrbytes = (char*)calloc(1,sizeof(fEvent));
    // copy the address stored in fEvent into the temporary buffer
    *reinterpret_cast<T**>(ptrbytes) = fEvent;
    // prepare the events smart buffer payload
    auto eventbuffer = new TSmartBuffer(ptrbytes,sizeof(fEvent));
    // set the pointer address into the buffer
    event.SetData(*eventbuffer);
    // set the timestamp of the ttree event
    if (timestamps.size()==0) {
      std::cout << "End of time stamps" << std::endl;
      return -1;
    }
    fEvent->SetTimestamp(timestamps.front());
    event.SetFragmentTimestamp(timestamps.front());
    timestamps.pop();

    // increment the event count
    fCurrentEntry++;
    //fEvent = nullptr;
    return sizeof(fEvent);
  }

  TChain fChain;
  kFileType fFileType;
  long fNumEvents;
  long fFileSize;
  T* fEvent;
  long fCurrentEntry;
  queue<Long_t> timestamps;

  ClassDef(TTreeSource,0);
};

#endif













#else // if RCNP is not defined


#ifndef _TTREESOURCE_H_
#define _TTREESOURCE_H_
#include "TObject.h"
#include "TRawEvent.h"
#include "TRawSource.h"
templateClassImp(TTreeSource)
template <typename T>
class TTreeSource : public TRawEventSource {
public:
  TTreeSource(const char* filename, const char* treename, const char* eventclassname, kFileType file_type) {;}
  ~TTreeSource() {;}
  virtual std::string Status() const { return ""; }
  virtual std::string SourceDescription() const {return ""; }
  kFileType GetFileType() const { return kFileType::UNKNOWN_FILETYPE; }
  long GetFileSize() const { return 0; }
  virtual void Reset() {;}
protected:
  void SetFileSize(long file_size) { ; }
private:
  TTreeSource() {;}
  virtual int GetEvent(TRawEvent& event) { event.SetFragmentTimestamp(0); return -1; }
  ClassDef(TTreeSource,0);
};
// ---------------------
class RCNPEvent : public TObject { };
// ---------------------
#endif


#endif // RCNP
