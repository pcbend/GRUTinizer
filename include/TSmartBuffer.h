#ifndef _TSMARTBUFFER_H_
#define _TSMARTBUFFER_H_

#ifndef __CINT__
#   include <mutex>
#endif

#include "TObject.h"

class TSmartBuffer : public TObject {
public:
  TSmartBuffer();
  TSmartBuffer(char* buffer, size_t size);
  ~TSmartBuffer();
  TSmartBuffer(const TSmartBuffer& other);
  TSmartBuffer& operator=(TSmartBuffer other);

  void Clear();

  void SetBuffer(char* buffer, size_t size);
  const char* GetData() const { return fData; }
  size_t GetSize()      const { return fSize; }

  TSmartBuffer BufferSubset(size_t pos, size_t length = -1) const;

#ifndef __CINT__
  TSmartBuffer(TSmartBuffer&& other);
#endif

private:
  void swap(TSmartBuffer& other);

  char* fAllocatedLocation;
  char* fData;
  size_t fSize;
#ifndef __CINT__
  int* fReferenceCount;
  std::mutex* fReferenceMutex;
#endif

  ClassDef(TSmartBuffer,0);
};

#endif /* _TSMARTBUFFER_H_ */
