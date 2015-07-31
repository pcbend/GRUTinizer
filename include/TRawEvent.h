#ifndef TRAWEVENT_H
#define TRAWEVENT_H

#include <iostream>
#include <iomanip>

#include "TObject.h"

#include "TRawFile.h"
#include "TSmartBuffer.h"
#include "TGRUTTypes.h"

/// Class holding a single event.
class TRawEvent : public TObject {

public:
#include "TRawBanks.h"

public:
  /// Constructs an empty TRawEvent.
  TRawEvent();
  /// Copy constructor
  TRawEvent(const TRawEvent&);
  virtual ~TRawEvent();
  TRawEvent& operator=(const TRawEvent &);

  virtual void  Clear(Option_t *opt ="");
  virtual Int_t Compare(const TRawEvent&) const;
  virtual void  Copy(const TRawEvent&)    ;
  virtual void  Print(Option_t *opt ="")  const;

  RawHeader *GetRawHeader();
  const char *GetBody() const;
  TSmartBuffer& GetBuffer(){ return fBody; }

   void SetData(TSmartBuffer body);

  bool IsGoodSize() const;
  void SetFileType(kFileType type) { fFileType = type; }
  kFileType GetFileType() { return fFileType; }

  Int_t  GetEventType() const;
  Int_t  GetTotalSize() const;
  Int_t  GetBodySize()  const;

protected:
  RawHeader fEventHeader;
  kFileType fFileType;
  TSmartBuffer fBody;

  ClassDef(TRawEvent,0)
};

#endif
