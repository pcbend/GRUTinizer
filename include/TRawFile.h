#ifndef TRAWFILE_H
#define TRAWFILE_H

#include "Globals.h"
#include "TGRUTTypes.h"

#include "TObject.h"
#include "TStopwatch.h"

#include "TSmartBuffer.h"

class TRawEvent;

class TRawFile {
public:
  TRawFile();
  virtual ~TRawFile();

  virtual bool Open(const char* fname, kFileType file_type) = 0;
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

protected:

  std::string fFilename;
  kFileType   fFileType;

  void Init();

  int         fLastErrno;
  std::string fLastError;

  mutable size_t fFileSize;
  size_t fBytesRead;
  size_t fBytesWritten;

  int     fFile;
  void*   fGzFile;
  FILE*   fPoFile;
  int     fOutFile;
  void*   fOutGzFile;

  mutable TStopwatch clock;

  ClassDef(TRawFile,0);
};

class TRawEventSource {
public:
  TRawEventSource() : fBytesGiven(0) { }
  virtual ~TRawEventSource() { }

  virtual int Read(TRawEvent* event);

  // Child classes must implement all virtual methods present.
  // Not using "= 0" because root complains if we do.
  virtual bool IsFinished() const { }
  virtual std::string SourceDescription() const { }
  virtual std::string Status() const { }
  virtual int GetLastErrno() const { }
  virtual std::string GetLastError() const { }

  size_t GetBytesGiven() const { return fBytesGiven; }
  void SetBytesGiven(size_t bytes = 0) { fBytesGiven = bytes; }

private:
  virtual int GetEvent(TRawEvent*) { }

  size_t fBytesGiven;

  ClassDef(TRawEventSource, 0);
};

class TRawFileIn  : public TRawFile, public TRawEventSource {
public:
  TRawFileIn() : fBufferSize(8192)  { }
  TRawFileIn(const char *fname, kFileType file_type);
  TRawFileIn(const char *fname);

  virtual ~TRawFileIn()      { }

  using TRawEventSource::Read;
  virtual bool IsFinished() const { return fIsFinished; }
  virtual std::string SourceDescription() const;
  virtual std::string Status() const;
  virtual int GetLastErrno() const { return TRawFile::GetLastErrno(); }
  virtual std::string GetLastError() const { return TRawFile::GetLastError(); }

  bool Open(const char *fname, kFileType file_type);

  void SetBufferSize(size_t buffer_size) { fBufferSize = buffer_size; }
  size_t GetBufferSize() { return fBufferSize; }

private:
  virtual int GetEvent(TRawEvent*);
  int FillBuffer(size_t bytes_requested);

  TSmartBuffer fCurrentBuffer;
  size_t fBufferSize;
  bool fIsFinished;

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
