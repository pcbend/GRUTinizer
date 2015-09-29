#ifndef TORDEREDRAWFILE_H
#define TORDEREDRAWFILE_H

#include "TRawFile.h"
#include "TGEBEvent.h"

class TOrderedRawFile : public TRawFileIn {

  public:
    TOrderedRawFile(const char* filename,kFileType file_type = kFileType::UNKNOWN_FILETYPE);
    ~TOrderedRawFile();

    virtual int Read(TRawEvent*);
    virtual bool IsFinished() const;
    void SetDepth(int depth) { this->depth = depth; }
    int GetDepth() { return depth; }

  private:
    int depth;
    long oldest_timestamp,newest_timestamp;
    bool finished;

    int fillqueue();

    mutable std::multimap<long,TGEBEvent> event_queue;

  ClassDef(TOrderedRawFile,0)
};


#endif
