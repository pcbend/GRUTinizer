#ifndef _TRAWEVENTSOURCE_H_
#define _TRAWEVENTSOURCE_H_

#include "Globals.h"

#include <cstdio>
#include <ctime>
#include <deque>
#include <string>

#include <zlib.h>

#include "TNamed.h"
#include "TStopwatch.h"

#include "TGRUTTypes.h"
#include "TGRUTOptions.h"
#include "TRawEvent.h"
#include "TSmartBuffer.h"



#include <fstream>
//#include <ios>

inline size_t FindFileSize(const char* fname) {
  ifstream temp;
  temp.open(fname, std::ios::in | std::ios::ate);
  size_t fsize = temp.tellg();
  temp.close();
  return fsize;
}




class TRawEventSource : public TObject  {
public:
  TRawEventSource()
    : fBytesGiven(0), fLastErrno(0), fIsFinished(0) { }
  virtual ~TRawEventSource() { }

  static TRawEventSource* EventSource(const char* filename,
                                      bool is_online = false, bool is_ring = false,
                                      kFileType file_type = DefaultFileType());

  /// Reads the next event.
  /** @param event The location of the event to be written.
      @returns The number of bytes read.
               If 0 is returned, no event has been read, but the source may provide an event later.
                 This is useful for online data analysis.
               If a negative number is returned, there was an error while reading,
                 possibly the end of file.
   */
  int Read(TRawEvent& event);
  int Read(TRawEvent* event);

  virtual std::string SourceDescription() const = 0;
  virtual std::string Status() const = 0;

  virtual int GetLastErrno() const { return fLastErrno; }
  virtual std::string GetLastError() const { return fLastError; }

  bool IsFinished() const { return fIsFinished; }
  size_t GetBytesGiven() const { return fBytesGiven; }
  double GetAverageRate() const;

  virtual void Reset();

protected:
  void SetLastErrno(int error) { fLastErrno = error; }
  void SetLastError(std::string error) { fLastError = error; }

private:
  /// Given a pointer to a TRawEvent, read the next event.
  /** @param event The event to be filled.
      @return The number of bytes that are read.
              Should return negative if there are no more events to be read.
   */
  virtual int GetEvent(TRawEvent& event) = 0;

  void UpdateByteThroughput(int bytes);

  static kFileType DefaultFileType();


  size_t fBytesGiven;
  int fLastErrno;
  bool fIsFinished;
  std::string fLastError;

  std::deque<size_t> fBytesPerSecond;
  time_t current_time;
  int seconds_to_average;

  ClassDef(TRawEventSource, 0);
};


class TRawEventByteSource : public TRawEventSource {
public:
  TRawEventByteSource(kFileType file_type);

  virtual std::string Status() const;


  kFileType GetFileType() const { return fFileType; }
  long GetFileSize() const { return fFileSize; }

  virtual void Reset();

protected:
  void SetFileSize(long file_size) { fFileSize = file_size; }

private:
  /// Given a buffer, fill the buffer with at most `size` bytes.
  /** @param buf The buffer to be filled.
      @param size The maximum number of bytes to be read.
      @return The number of bytes that were read.
              Should return zero at end of file.
              Should return a negative value in the case of error.
   */
  virtual int ReadBytes(char* buf, size_t size) = 0;

  virtual int GetEvent(TRawEvent& event);
  int FillBuffer(size_t bytes_requested);

  kFileType fFileType;

  long fFileSize;

  TSmartBuffer fCurrentBuffer;
  size_t fDefaultBufferSize;

  ClassDef(TRawEventByteSource,0);
};


class TRawEventPipeSource : public TRawEventByteSource {
public:
  TRawEventPipeSource(const std::string& command, kFileType file_type);
  ~TRawEventPipeSource();

  virtual int ReadBytes(char* buf, size_t size);
  virtual void Reset();

  virtual std::string SourceDescription() const;
private:
  std::string fCommand;
  FILE* fPipe;

  ClassDef(TRawEventPipeSource,0);
};


class TRawEventGZipSource : public TRawEventByteSource {
public:
  TRawEventGZipSource(const std::string& filename, kFileType file_type);
  ~TRawEventGZipSource();

  virtual int ReadBytes(char* buf, size_t size);
  virtual void Reset();

  virtual std::string SourceDescription() const;
private:
  std::string fFilename;
  FILE* fFile;
  gzFile* fGzFile;

  ClassDef(TRawEventGZipSource,0);
};


class TRawEventBZipSource : public TRawEventPipeSource {
public:
  TRawEventBZipSource(const std::string& filename, kFileType file_type);
  ~TRawEventBZipSource() { }

  virtual std::string SourceDescription() const;

private:
  std::string fFilename;

  ClassDef(TRawEventBZipSource,0);
};

class TRawEventFileSource : public TRawEventByteSource {
public:
  TRawEventFileSource(const std::string& filename, kFileType file_type);
  ~TRawEventFileSource();

  virtual int ReadBytes(char* buf, size_t size);
  virtual void Reset();

  virtual std::string SourceDescription() const;

private:
  void FastForward();

  std::string fFilename;
  FILE* fFile;

  ClassDef(TRawEventFileSource,0);
};


class TRawEventRingSource : public TRawEventPipeSource {
public:
  TRawEventRingSource(const std::string& ringname, kFileType file_type);
  ~TRawEventRingSource() { }

  virtual std::string SourceDescription() const;

private:
  std::string fRingName;

  ClassDef(TRawEventRingSource,0);
};


class TRawEventOnlineFileSource : public TRawEventByteSource {
public:
  TRawEventOnlineFileSource(const std::string& filename, kFileType file_type);
  ~TRawEventOnlineFileSource();

  virtual int ReadBytes(char* buf, size_t size);
  virtual void Reset();

  virtual std::string SourceDescription() const;
private:
  int single_read(char* buf, size_t size);

  std::string fFilename;
  FILE* fFile;

  ClassDef(TRawEventOnlineFileSource,0);
};



class TRawFile : public TRawEventSource { //,public TNamed {
public:
  TRawFile(const char* filename, kFileType file_type = kFileType::UNKNOWN_FILETYPE);
  ~TRawFile();

  virtual std::string SourceDescription() const {
    return wrapped->SourceDescription();
  }

  virtual std::string Status() const {
    return wrapped->Status();
  }

  virtual int GetLastErrno() const {
    return wrapped->GetLastErrno();
  }

  virtual std::string GetLastError() const {
    return wrapped->GetLastError();
  }

  virtual void Reset() {
    TRawEventSource::Reset();
    wrapped->Reset();
  }

  void SetNameTitle(const char *name,const char *title) {
    fname.assign(name);
    ftitle.assign(title);
  }
  void SetName(const char *name) { fname.assign(name);};
  void SetTitle(const char *title) { ftitle.assign(title);};

  const char *GetName() { return fname.c_str(); }
  const char *GetTitle() { return ftitle.c_str(); }

private:
  virtual int GetEvent(TRawEvent& event) {
    return wrapped->Read(event);
  }

  std::string fname;
  std::string ftitle;

  TRawEventSource* wrapped;
  ClassDef(TRawFile,1)
};

class TRawFileIn : public TRawFile {
public:
  TRawFileIn(const char* filename, kFileType file_type = kFileType::UNKNOWN_FILETYPE)
    : TRawFile(filename, file_type) { }

  TRawEvent PrintNext(Option_t *opt="all") { TRawEvent event; this->Read(&event); event.Print(opt); return event; }

  ClassDef(TRawFileIn,0)
};







#endif /* _TRAWEVENTSOURCE_H_ */
