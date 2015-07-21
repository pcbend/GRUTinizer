#ifndef TRAWEVENT_H
#define TRAWEVENT_H


#include "TObject.h"

#include "TRawFile.h"

class TRawEvent : public TObject {

  public:
    #include "TRawBanks.h"

  public:
    TRawEvent();
    TRawEvent(const TRawEvent&);
    ~TRawEvent();
    TRawEvent& operator=(const TRawEvent &);

    void  Clear(Option_t *opt ="");
    Int_t Compare(const TRawEvent&) const;
    void  Copy(const TRawEvent&)    ;
    void  Print(Option_t *opt ="")  const;



    RawHeader *GetRawHeader();
    char *GetData() const;

    void AllocateData() const;
    void SetData(Int_t dataSize, char *dataBuffer);

    bool IsGoodSize() const;
    void SetFileType(kFileType type) { fFileType = type; }

    //inline const Int_t  GetEventType() const { return fEventHeader.type; }
    //inline const Int_t  GetDataSize()  const { return fEventHeader.size; }
    //inline const Long_t GetTimeStamp() const { return fEventHeader.timestamp; }
    Int_t  GetEventType() const;
    Int_t  GetBodySize()  const;
    Long_t GetTimeStamp() const;

    bool operator<(const TRawEvent &rhs) const { return (GetTimeStamp() < rhs.GetTimeStamp()); }
    bool operator>(const TRawEvent &rhs) const { return (GetTimeStamp() > rhs.GetTimeStamp()); }

    void   AdvanceDataPtr(int x) { fData +=x; }
    Long_t fTimeStamp;
  protected:
    RawHeader fEventHeader;
    kFileType fFileType;
    mutable char *fData;
    mutable bool fAllocatedByUs;

  ClassDef(TRawEvent,0)
};

#endif
