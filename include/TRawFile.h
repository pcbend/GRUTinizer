#ifndef _TRAWFILE_H_
#define _TRAWFILE_H_

#include "TRawEventFileSource.h"
#include "TGRUTOptions.h"

class TRawFile : public TRawEventFileSource {
public:
  TRawFile(const std::string& filename, kFileType file_type = kFileType::UNKNOWN_FILETYPE)
    : TRawEventFileSource(filename,
                          (file_type==kFileType::UNKNOWN_FILETYPE) ?
                          TGRUTOptions::Get()->DetermineFileType(filename) :
                          file_type) { }
};

class TRawFileIn : public TRawEventFileSource {
public:
  TRawFileIn(const std::string& filename, kFileType file_type = kFileType::UNKNOWN_FILETYPE)
    : TRawEventFileSource(filename,
                          (file_type==kFileType::UNKNOWN_FILETYPE) ?
                          TGRUTOptions::Get()->DetermineFileType(filename) :
                          file_type) { }
};


#endif /* _TRAWFILE_H_ */
