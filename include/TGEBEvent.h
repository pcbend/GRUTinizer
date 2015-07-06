#ifndef TGEBEVENT_H
#define TGEBEVENT_H

  
#include "TObject.h"

#ifndef __CINT__
#include "TGEBEventHeader.h"
#else
class TGEBEventHeader;
#endif

//#include "TGEBBanks.h"

class TGEBEvent : public TObject {

  public: 
    #include "TGEBBanks.h"
    //#include "TGEBBanksFunctions.h"

  public:
    TGEBEvent();
    TGEBEvent(const TGEBEvent&);
    ~TGEBEvent();
    TGEBEvent& operator=(const TGEBEvent &);
   
    void  Clear(Option_t *opt ="");
    Int_t Compare(const TGEBEvent&) const;
    void  Copy(const TGEBEvent&)    ;
    void  Print(Option_t *opt ="")  const;
    


    TGEBEventHeader *GetEventHeader();
    char *GetData() const;

    void AllocateData() const;
    void SetData(Int_t dataSize, char *dataBuffer);

    bool IsGoodSize() const;

    //inline const Int_t  GetEventType() const { return fEventHeader.type; }
    //inline const Int_t  GetDataSize()  const { return fEventHeader.size; }
    //inline const Long_t GetTimeStamp() const { return fEventHeader.timestamp; }
    Int_t  GetEventType() const;
    Int_t  GetDataSize()  const;
    Long_t GetTimeStamp() const;


    bool operator<(const TGEBEvent &rhs) const { return (GetTimeStamp() < rhs.GetTimeStamp()); }
    bool operator>(const TGEBEvent &rhs) const { return (GetTimeStamp() > rhs.GetTimeStamp()); }

  protected:
    TGEBEventHeader fEventHeader;
    mutable char *fData;
    mutable bool fAllocatedByUs;

  ClassDef(TGEBEvent,0)
};

#endif


