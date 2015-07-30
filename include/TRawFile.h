#ifndef TRAWFILE_H
#define TRAWFILE_H

#include "Globals.h"
#include "TGRUTTypes.h"

#include "TObject.h"
#include "TStopwatch.h"

#include "TSmartBuffer.h"

class TRawEvent;

class TRawFile : public TObject {
public:
  TRawFile();
  virtual ~TRawFile();

  virtual bool Open(const char* fname, kFileType file_type) { AbstractMethod("Open()"); }
  void Close();

  const char * GetFileName()  const { return fFilename.c_str();  }
  int          GetLastErrno() const { return fLastErrno;         }
  const char * GetLastError() const { return fLastError.c_str(); }

  void Print(Option_t *opt = "") const;
  void Clear(Option_t *opt = "");

  size_t GetFileSize() const;
  static size_t FindFileSize(const char*);

  const kFileType GetFileType() const               { return fFileType; }
        void      SetFileType(const kFileType type) { fFileType = type; }

  std::string Status() const;
  bool IsFinished() const;

protected:

  std::string fFilename;
  kFileType   fFileType;

  void Init();

  int         fLastErrno;
  std::string fLastError;
  bool        fIsFinished;

  mutable size_t fFileSize;
  size_t fBytesRead;
  size_t fBytesGiven;
  size_t fBytesWritten;

  int     fFile;
  void*   fGzFile;
  FILE*   fPoFile;
  int     fOutFile;
  void*   fOutGzFile;

  mutable TStopwatch clock;

  ClassDef(TRawFile,0);
};



class TRawFileIn  : public TRawFile {
public:
  TRawFileIn() : fBufferSize(8192)  { }
  TRawFileIn(const char *fname, kFileType file_type);
  TRawFileIn(const char *fname);

  virtual ~TRawFileIn()      { }


  bool Open(const char *fname, kFileType file_type);
  int Read(TRawEvent*);
  TRawEvent Read();

  void SetBufferSize(size_t buffer_size) { fBufferSize = buffer_size; }
  size_t GetBufferSize() { return fBufferSize; }

private:
  int FillBuffer(size_t bytes_requested);

  TSmartBuffer fCurrentBuffer;
  size_t fBufferSize;

  ClassDef(TRawFileIn,0);
};


class TRawFileOut : public TRawFile {
public:
  TRawFileOut() : TRawFile() { }
  virtual ~TRawFileOut()     { }

  TRawFileOut(const char *fname, kFileType file_type);
  bool Open(const char *fname, kFileType file_type);
  int Write(TRawEvent*);

private:

  ClassDef(TRawFileOut,0);
};

#endif
