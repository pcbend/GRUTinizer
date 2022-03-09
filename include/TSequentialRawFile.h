#ifndef _TSEQUENTIALRAWFILE_H_
#define _TSEQUENTIALRAWFILE_H_

#include "TRawSource.h"

class TSequentialRawFile : public TRawEventSource {
public:
  TSequentialRawFile();
  ~TSequentialRawFile();

  void Add(TRawEventSource* infile);
  virtual void Reset();

  virtual std::string SourceDescription(bool long_description = false) const;
  virtual std::string Status(bool long_description = false) const;
  virtual int GetLastErrno() const;
  virtual std::string GetLastError() const;


private:
  // TODO, can I disable thse somehow without annoying ROOT?
  TSequentialRawFile(const TSequentialRawFile& other) { }
  TSequentialRawFile& operator=(const TSequentialRawFile& other) { return *this; }

  virtual int GetEvent(TRawEvent& event);

  size_t active_source;
  std::vector<TRawEventSource*> sources;

  ClassDef(TSequentialRawFile, 0);
};

#endif /* _TSEQUENTIALRAWFILE_H_ */
