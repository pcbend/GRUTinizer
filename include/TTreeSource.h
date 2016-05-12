#ifndef _TTREESOURCE_H_
#define _TTREESOURCE_H_

#include <vector>
#include <cassert>

#include "TObject.h"
#include "TRawEvent.h"
#include "TRawSource.h"
#include "TChain.h"

templateClassImp(TTreeSource)

template <typename T>
class TTreeSource : public TRawEventSource {
public:

  /* TTreeSource(const char* filename, const char* treename, const char* eventclassname, kFileType file_type) */
  /*   : TTreeSource<T>({filename},treename,eventclassname,file_type) { ; } */

  template<typename... Args>
  TTreeSource(const char* filename, const char* treename, const char* eventclassname, kFileType file_type, Args&&... args)
    : TTreeSource<T>({filename},treename,eventclassname,file_type,std::forward<Args>(args)...) { ; }

  template<typename... Args>
  TTreeSource(const std::vector<char*>& filenames, const char* treename, const char* eventclassname, kFileType file_type, Args&&... args)
    : fChain(treename), fCurrentEntry(0) {

    assert(file_type == kFileType::ROOT_DATA);
    fFileType = file_type;


    for (auto fn : filenames) {
      fChain.Add(fn);
    }
    fFileSize =  fChain.GetEntries()*sizeof(T);

    if (sizeof...(Args) > 0) {
      fEvent = new T(std::forward<Args>(args)...);
    } else {
      fEvent = new T();
    }
    fChain.SetBranchAddress(eventclassname, &fEvent);
  }

  ~TTreeSource() {;}

  virtual std::string Status() const {return std::string("");}
  virtual std::string SourceDescription() const {return std::string("");}


  kFileType GetFileType() const { return fFileType; }
  long GetFileSize() const { return fFileSize; }

  virtual void Reset() {;}

protected:
  void SetFileSize(long file_size) { fFileSize = file_size; }

private:
  TTreeSource() {;}
  virtual int GetEvent(TRawEvent& event) {
    event.SetFileType(fFileType);

    // copy construct a new object from the old
    fEvent = new T(*fEvent);
    // fill the new object with current event data
    fChain.GetEntry(fCurrentEntry);
    // create a small memory buffer to hold the pointer to the current entry
    char* ptrbytes = (char*)malloc(sizeof(fEvent));
    memcpy(ptrbytes, &fEvent, sizeof(fEvent));
    TSmartBuffer eventbuffer(ptrbytes,sizeof(fEvent));
    // set the pointer address into the buffer
    event.SetData(eventbuffer);
    // set the timestamp of the ttree event
    event.SetFragmentTimestamp(fEvent->GetTimestamp());

    fCurrentEntry++;
    //fEvent = nullptr;
    return sizeof(*fEvent);
  }

  TChain fChain;
  kFileType fFileType;
  long fFileSize;
  T* fEvent;
  long fCurrentEntry;

  ClassDef(TTreeSource,0);
};

/* Prototype example, to be removed */
class EventType : public TObject {
public:
  EventType(){;}
  virtual ~EventType(){;}
  long GetTimestamp() {return fTimestamp;}
  long fTimestamp;
  ClassDef(EventType,0);
};

/* template<typename T, typename... Args> TTreeSource<T> make_ttree_source(const char* filename, const char* treename, Args&&... args) */
/* { */

/* } */
/* make_ttree_source<BranchType>("./rootfilename.root","treename","branchname"); */
/* make_ttree_source<BranchType1,BranchType2>("./rootfilename.root","treename","branch1name","branch2name"); */
/* make_ttree_source<BranchType1,BranchType2>("./rootfilename.root","treename","branch1name", ... ,"branch2name", ...); */


#endif
