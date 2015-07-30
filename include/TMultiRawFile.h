#ifndef _TMULTIRAWFILE_H_
#define _TMULTIRAWFILE_H_

#include <set>

#include "TObject.h"

#include "TGEBEvent.h"
#include "TRawFile.h"

struct FileEvent {
  TRawFileIn* file;
  TGEBEvent next_event;

  // Needed for keeping the std::set sorted by timestamp
  bool operator<(const FileEvent& other) const;
};

bool file_event_sorting(const FileEvent& a, const FileEvent& b);

class TMultiRawFile : public TObject {
public:
  TMultiRawFile();
  ~TMultiRawFile();

  void AddFile(TRawFileIn* infile);
  void AddFile(const char* filename);
  int Read(TGEBEvent* outevent);

private:
  std::set<FileEvent> fFileEvents;

  ClassDef(TMultiRawFile, 0);
};




#endif
