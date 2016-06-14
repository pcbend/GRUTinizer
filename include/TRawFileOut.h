#ifndef _TRAWFILEOUT_H_
#define _TRAWFILEOUT_H_

#include <cstdio>
#include <fstream>
#include <string>

#include <zlib.h>

#include "TObject.h"

class TRawEvent;
class TUnpackedEvent;

class TRawFileOut : public TObject {
public:
  TRawFileOut();
  explicit TRawFileOut(const std::string& filename);
  ~TRawFileOut();

  void Open(const std::string& filename);

  void Write(TRawEvent& event);
  void Write(TUnpackedEvent& event);

private:
  // Because CINT doesn't understand C++11.
  // Once we switch to ROOT6, the __CINT__ workaround can be removed.
#ifdef __CINT__
  TRawFileOut(const TRawFileOut&);
#else
  TRawFileOut(const TRawFileOut&) = delete;
#endif
  void swap(TRawFileOut& other);
  void WriteBytes(const char* data, size_t size);

  void WriteUnbuiltEvent(TUnpackedEvent& event);
  void WriteBuiltNSCLEvent(TUnpackedEvent& event);

  FILE* raw_file_out;
  gzFile* gzip_file_out;

  ClassDef(TRawFileOut, 0);
};

#endif /* _TRAWFILEOUT_H_ */
