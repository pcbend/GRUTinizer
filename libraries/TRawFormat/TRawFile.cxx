#include "TRawFile.h"

#include <cstdio>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <cstdlib>
#include <fstream>

#include <zlib.h>

#include <Rtypes.h>

#include "TRawEvent.h"

ClassImp(TRawFile)


TRawFile::TRawFile() {
  Init();
  Close();
}


TRawFileIn::TRawFileIn(const char *fname, kFileType file_type)
  : fBufferSize(8192) {
  Open(fname, file_type);
}

TRawFileOut::TRawFileOut(const char *fname, kFileType file_type) {
  Open(fname, file_type);
}

TRawFile::~TRawFile() {
  Close();
}

void TRawFile::Init() {
  fFilename = "";
  fFileType = kFileType::UNKNOWN;

  fFile      = -1;
  fGzFile    = NULL;
  fPoFile    = NULL;
  fLastErrno = 0;

  fFileSize  = -1;

  fOutFile   = -1;
  fOutGzFile = NULL;
}

static int hasSuffix(const char *name,const char *suffix) {
  const char *s = strstr(name,suffix);
  if(s==NULL)
    return 0;
  return (s-name)+strlen(suffix) == strlen(name);
}

bool TRawFileIn::Open(const char *filename, kFileType file_type) {
  // currently able to read:
  //    normal .dat file
  //    bzip file ->   .bz2
  //    gzip fuke ->   .gz



  if(fFile>0)
    Close();

  fFilename = filename;
  std::string pipe;

  if(hasSuffix(filename,".bz2")) {
    pipe = "bzip2 -dc ";
    pipe += filename;
  }
  if(pipe.length()>0) {
    fprintf(stderr,"TRawFile::Open: Reading from pipe: %s\n",pipe.c_str());
    fPoFile = popen(pipe.c_str(),"r");

    if(fPoFile == NULL) {
      fLastErrno = errno;
      fLastError = strerror(errno);
      SetFileType(kFileType::UNKNOWN);
      return false;
    }

    fFile = fileno((FILE*)fPoFile);
  } else {
     printf("File name:  %s\n",filename);
     fFile = open(filename, O_RDONLY | O_LARGEFILE);

     if(fFile<=0) {
       fLastErrno = errno;
       fLastError = strerror(errno);
       SetFileType(kFileType::UNKNOWN);
       return false;
     }

     if(hasSuffix(filename,".gz")) {
       fGzFile = new gzFile;
       (*(gzFile*)fGzFile) = gzdopen(fFile,"rb");

       if((*(gzFile*)fGzFile)==NULL) {
         fLastErrno = -1;
         fLastError = "zlib gzdopen() error";
         SetFileType(kFileType::UNKNOWN);
         return false;
       }
     }
  }
  SetFileType(file_type);
  return true;
}

bool TRawFileOut::Open(const char *filename, kFileType file_type) {

  if(fOutFile>0)
    Close();

  fFilename = filename;
  fOutFile = open(filename,O_WRONLY | O_CREAT | O_TRUNC | O_LARGEFILE, 0644);

  if(fOutFile <= 0) {
    fLastErrno = errno;
    fLastError = strerror(errno);
    SetFileType(kFileType::UNKNOWN);
    return false;
  }

  if(hasSuffix(filename,".gz")) {
    fOutGzFile = new gzFile;
    *(gzFile*)fOutGzFile = gzdopen(fOutFile,"wb");
    if((*(gzFile*)fOutGzFile) == NULL) {
      fLastErrno = -1;
      fLastError = "zlib gzdopen() error";
      SetFileType(kFileType::UNKNOWN);
      return false;
    }
    if(gzsetparams(*(gzFile*)fOutGzFile,1,Z_DEFAULT_STRATEGY) != Z_OK) {
      fLastErrno = -1;
      fLastError = "zlib gzsetparams() error";
      SetFileType(kFileType::UNKNOWN);
      return false;
    }

  }
  SetFileType(file_type);
  return true;
}

static int readpipe(int fd, char *buf, int length) {
  int count = 0;
  while(length>0) {
    int rd = read(fd,buf,length);
    if(rd>0) {
      buf += rd;
      length -= rd;
      count += rd;
    } else if(rd==0) {
      return count;
    } else {
      return -1;
    }
  }
  return count;
}

TRawEvent TRawFileIn::Read(){
  TRawEvent output;
  Read(&output);
  return output;
}

int TRawFileIn::Read(TRawEvent *rawevent) {

   if(!rawevent) {
    fprintf(stderr,"TGEBFILE::Read;  Trying to filee NULL TRawEvent.");
    return -1;
   }

   switch(GetFileType()) {
     case kFileType::NSCL_EVT:
     case kFileType::GRETINA_MODE2:
       break;
     case kFileType::GRETINA_MODE3:
       printf("I do not know how to read GEB Mode3 data yet.\n");
       return 0;
       break;
     case kFileType::UNKNOWN:
       printf("I do not know how to read Unknown data yet.\n");
       return 0;
       break;
   }

   rawevent->Clear();
   rawevent->SetFileType(GetFileType());

   int bytes_read_header = FillBuffer(sizeof(TRawEvent::RawHeader));
   if(bytes_read_header < 0){
     return -1;
   }

   memcpy(rawevent->GetRawHeader(), fCurrentBuffer.GetData(), sizeof(TRawEvent::RawHeader));
   fCurrentBuffer = fCurrentBuffer.BufferSubset(sizeof(TRawEvent::RawHeader));

   size_t body_size = rawevent->GetBodySize();
   int bytes_read = FillBuffer(body_size);
   if(bytes_read < 0){
     return -2;
   }

   rawevent->SetData(fCurrentBuffer.BufferSubset(0, body_size));
   fCurrentBuffer = fCurrentBuffer.BufferSubset(body_size);

   if(!rawevent->IsGoodSize()) {
     fLastErrno = -1;
     fLastError = "Invalid event size";
     return -1;
   }

   return sizeof(TRawEvent::RawHeader) + body_size;
}

int TRawFileIn::FillBuffer(size_t bytes_requested) {
  if(fCurrentBuffer.GetSize() >= bytes_requested){
    return 0;
  }

  char* buf = (char*)malloc(fBufferSize);

  // Copy any leftover bytes from the previous buffer.
  size_t bytes_to_copy = fCurrentBuffer.GetSize();
  memcpy(buf, fCurrentBuffer.GetData(), bytes_to_copy );

  // Read to fill the buffer.
  size_t bytes_to_read = fBufferSize - bytes_to_copy;
  size_t bytes_read;
  if(fGzFile) {
    bytes_read = gzread(*(gzFile*)fGzFile, buf + bytes_to_copy, bytes_to_read);
  } else {
    bytes_read = readpipe(fFile, buf + bytes_to_copy, bytes_to_read);
  }

  // Store everything
  fCurrentBuffer = TSmartBuffer(buf, bytes_to_copy + bytes_read);
  fBytesRead += bytes_read;

  // Set the error flags and return code appropriately.
  if(bytes_read == 0){
    fLastErrno = 0;
    fLastError = "EOF";
    return 1;
  } else if (bytes_read < bytes_requested){
    fLastErrno = errno;
    fLastError = strerror(errno);
    return 2;
  } else {
    return 0;
  }
}

int TRawFileOut::Write(TRawEvent *rawevent) {
  int wr = -2;
  if(fOutGzFile) {
    wr = gzwrite(*(gzFile*)fOutGzFile,(char*)rawevent->GetRawHeader(),sizeof(TRawEvent::RawHeader));
  } else {
    wr = write(fOutFile,(char*)rawevent->GetRawHeader(),sizeof(TRawEvent::RawHeader));
  }

  if(wr != sizeof(TRawEvent::RawHeader)) {
    fprintf(stderr,"TRawFile: write header error; return %i, size requested %lu\n",wr,sizeof(TRawEvent::RawHeader));
    return -1;
  }

  if(fOutGzFile)
    wr = gzwrite(*(gzFile*)fOutGzFile, rawevent->GetBody(), rawevent->GetBodySize());
  else
    wr = write(fOutFile, rawevent->GetBody(), rawevent->GetBodySize());


  if(wr != rawevent->GetBodySize()) {
    fprintf(stderr,"TRawFile: write data error; return %i, size requested %lu\n",wr,rawevent->GetBodySize());
    return -1;
  }

  return wr;
}

void TRawFile::Close() {
  //Any input files
  if(fPoFile)
    pclose((FILE*)fPoFile);
  fPoFile = NULL;

  if(fGzFile){
    gzclose(*(gzFile*)fGzFile);
    delete (gzFile*)fGzFile;
  }
  fGzFile = NULL;

  //Any output files
  if(fFile>0)
    close(fFile);
  fFile = -1;

  if(fOutGzFile) {
    gzflush(*(gzFile*)fOutGzFile,Z_FULL_FLUSH);
    gzclose(*(gzFile*)fOutGzFile);
  }
  fOutGzFile = NULL;
  if(fOutFile>0)
    close(fOutFile);
  fOutFile = -1;

  //And reset
  Init();
}

void TRawFile::Clear(Option_t *opt) { }

void TRawFile::Print(Option_t *opt) const { }

size_t TRawFile::FindFileSize(const char* fname) {
  ifstream temp;
  temp.open(fname, std::ios::in | std::ios::ate);
  size_t fsize = temp.tellg();
  temp.close();
  return fsize;
}

size_t TRawFile::GetFileSize() {
  if(fFileSize==-1)
    fFileSize = FindFileSize(GetFileName());
  return fFileSize;
}
