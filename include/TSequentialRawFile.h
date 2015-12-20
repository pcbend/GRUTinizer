#ifndef _TSEQUENTIALRAWFILE_H_
#define _TSEQUENTIALRAWFILE_H_

#include "TRawSource.h"

class TSequentialRawFile : public TRawEventSource {
public:
  TSequentialRawFile();
  ~TSequentialRawFile();
  // TODO, can I disable thse somehow without annoying ROOT?
  TSequentialRawFile(const TSequentialRawFile& other) { }
  TSequentialRawFile& operator=(const TSequentialRawFile& other) { }

  void Add(TRawEventSource* infile);

  virtual std::string SourceDescription() const;
  virtual std::string Status() const;
  virtual int GetLastErrno() const;
  virtual std::string GetLastError() const;


private:
  virtual int GetEvent(TRawEvent& event);

  int active_source;
  std::vector<TRawEventSource*> sources;

  ClassDef(TSequentialRawFile, 0);
};

#endif /* _TSEQUENTIALRAWFILE_H_ */
