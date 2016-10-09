#ifndef TRAWEVENT_H
#define TRAWEVENT_H

#include <Globals.h>

#include <iostream>
#include <iomanip>

#include "TObject.h"

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
  virtual Int_t Compare(const TObject&) const;
  virtual void  Copy(TObject&) const;
  virtual void  Print(Option_t *opt ="")  const;

  RawHeader *GetRawHeader();
  const char *GetBody() const;
  TSmartBuffer& GetBuffer(){ return fBody; }

  const char* GetPayload() const;
  TSmartBuffer GetPayloadBuffer() const;

  void SetData(TSmartBuffer body);

  bool IsGoodSize() const;
  void SetFileType(kFileType type) { fFileType = type; }
  kFileType GetFileType() { return fFileType; }

  Int_t  GetEventType() const;
  Int_t  GetTotalSize() const;
  Int_t  GetBodySize()  const;
  Long_t GetTimestamp() const;

  void SetFragmentTimestamp(long timestamp) { fTimestamp = timestamp; }

  void SetDataPtr(void* ptr) { fDataPtr = ptr; }
  void* GetDataPtr() { return fDataPtr; }

protected:
  RawHeader fEventHeader;
  kFileType fFileType;
  long fTimestamp;
  TSmartBuffer fBody;
  void* fDataPtr;


  ClassDef(TRawEvent,0)
};

#endif
