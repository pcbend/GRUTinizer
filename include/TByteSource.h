#ifndef _TBYTESOURCE_H_
#define _TBYTESOURCE_H_

#include <string>

#include <zlib.h>

class TByteSource {
public:
  TByteSource();
  virtual ~TByteSource() { }

  virtual int ReadBytes(char* buf, size_t size) = 0;
  virtual void Reset() = 0;
  virtual std::string SourceDescription(bool long_description = false) const = 0;

  virtual int GetLastErrno() const { return fLastErrno; }
  virtual std::string GetLastError() const { return fLastError; }

  virtual long GetFileSize() const { return fFileSize; }

protected:
  void SetLastErrno(int error) { fLastErrno = error; }
  void SetLastError(std::string error) { fLastError = error; }
  void SetFileSize(long file_size) { fFileSize = file_size; }

private:
  int fLastErrno;
  std::string fLastError;
  long fFileSize;
};

class TFileByteSource : public TByteSource {
public:
  TFileByteSource(const std::string& filename);
  ~TFileByteSource();

  virtual int ReadBytes(char* buf, size_t size);
  virtual void Reset();

  virtual std::string SourceDescription(bool long_description=false) const;
private:
  std::string fFilename;
  FILE* fFile;
};

class TPipeByteSource : public TByteSource {
public:
  TPipeByteSource(const std::string& command);
  ~TPipeByteSource();

  virtual int ReadBytes(char* buf, size_t size);
  virtual void Reset();

  virtual std::string SourceDescription(bool long_description=false) const;
private:
  std::string fCommand;
  FILE* fPipe;
};


class TGZipByteSource : public TByteSource {
public:
  TGZipByteSource(const std::string& filename);
  ~TGZipByteSource();

  virtual int ReadBytes(char* buf, size_t size);
  virtual void Reset();

  virtual std::string SourceDescription(bool long_description=false) const;
private:
  std::string fFilename;
  FILE* fFile;
  gzFile* fGzFile;
};


class TBZipByteSource : public TPipeByteSource {
public:
  TBZipByteSource(const std::string& filename);
  ~TBZipByteSource() { }

  virtual std::string SourceDescription(bool long_description=false) const;

private:
  std::string fFilename;
};


class TRingByteSource : public TPipeByteSource {
public:
  TRingByteSource(const std::string& ringname);
  ~TRingByteSource() { }

  virtual std::string SourceDescription(bool long_description=false) const;

private:
  std::string fRingName;
};

#endif /* _TBYTESOURCE_H_ */
