#ifndef _TRAWEVENTSOURCE_H_
#define _TRAWEVENTSOURCE_H_

#include "Globals.h"

#include <cstdio>
#include <ctime>
#include <deque>
#include <string>

#include "TStopwatch.h"

#include "TGRUTTypes.h"
#include "TGRUTOptions.h"
#include "TRawEvent.h"
#include "TSmartBuffer.h"


class TRawEventSource {
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
               If 0 is returned, the source has been read to completion.
               If a negative number is returned, there was an error while reading.
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
  mutable TStopwatch clock;

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

  virtual std::string SourceDescription() const;
private:
  std::string fCommand;
  FILE* fPipe;

  ClassDef(TRawEventPipeSource,0);
};

#endif /* _TRAWEVENTSOURCE_H_ */
