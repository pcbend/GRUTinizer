#ifndef TRAWEVENT_H
#define TRAWEVENT_H

#include <iostream>
#include <iomanip>

#include "TObject.h"

#include "TRawFile.h"

class TRawEvent : public TObject {

public:
#include "TRawBanks.h"

public:
  TRawEvent();
  TRawEvent(const TRawEvent&);
  virtual ~TRawEvent();
  TRawEvent& operator=(const TRawEvent &);

  void  Clear(Option_t *opt ="");
  Int_t Compare(const TRawEvent&) const;
  void  Copy(const TRawEvent&)    ;
  void  Print(Option_t *opt ="")  const;

  RawHeader *GetRawHeader();
  char *GetBody() const;

  void AllocateData() const;
  void SetData(Int_t dataSize, char *dataBuffer);

  bool IsGoodSize() const;
  void SetFileType(kFileType type) { fFileType = type; }

  Int_t  GetEventType() const;
  Int_t  GetTotalSize() const;
  Int_t  GetBodySize()  const;

protected:
  RawHeader fEventHeader;
  kFileType fFileType;
  mutable char   *fData;
  mutable bool    fAllocatedByUs;

  ClassDef(TRawEvent,0)
};

#endif
