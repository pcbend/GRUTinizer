#ifndef _TRAWEVENTSOURCE_H_
#define _TRAWEVENTSOURCE_H_

#include "Globals.h"

#include <cstdio>
#include <ctime>
#include <deque>
#include <fstream>
#include <string>

#include "TByteSource.h"
#include "TGRUTTypes.h"
#include "TGRUTOptions.h"
#include "TGRUTUtilities.h"
#include "TRawEvent.h"
#include "TSmartBuffer.h"

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

  virtual std::string SourceDescription(bool long_description=false) const = 0;
  virtual std::string Status(bool long_description = false) const = 0;

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


class TRawEventTimestampSource : public TRawEventSource {
public:
  TRawEventTimestampSource(TByteSource* byte_source, kFileType file_type);
  ~TRawEventTimestampSource();

  virtual std::string Status(bool long_description = false) const;
  virtual void Reset();
  virtual std::string SourceDescription(bool long_description=false) const {
    return fByteSource->SourceDescription();
  }

  virtual int GetLastErrno() const { return fByteSource->GetLastErrno(); }
  virtual std::string GetLastError() const { return fByteSource->GetLastError(); }

  kFileType GetFileType() const { return fFileType; }
  long GetFileSize() const { return fByteSource->GetFileSize(); }

private:
  virtual int GetEvent(TRawEvent& event);
  int FillBuffer(size_t bytes_requested);

  TByteSource* fByteSource;
  kFileType fFileType;

  TSmartBuffer fCurrentBuffer;
  size_t fDefaultBufferSize;

  ClassDef(TRawEventTimestampSource,0);
};





class TRawFile : public TRawEventSource {
public:
  TRawFile(const char* filename, kFileType file_type = kFileType::UNKNOWN_FILETYPE);
  ~TRawFile();

  virtual std::string SourceDescription(bool long_description=false) const {
    return wrapped->SourceDescription(long_description);
  }

  virtual std::string Status(bool long_description = false) const {
    return wrapped->Status(long_description);
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
