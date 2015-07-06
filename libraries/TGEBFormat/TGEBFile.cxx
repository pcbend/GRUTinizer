
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

#include "TGEBFile.h"
#include "TGEBEvent.h"

ClassImp(TGEBFile)


TGEBFile::TGEBFile() { 
  InitTGEBFile();
  Close();
  CloseOut();
}

TGEBFile::TGEBFile(const char *fname) {
  InitTGEBFile();
  Close();
  CloseOut();
  Open(fname);
}

TGEBFile::~TGEBFile() {   
  Close();
  CloseOut();
}

void TGEBFile::InitTGEBFile() {
  
  fFile      = -1;
  fGzFile    = NULL;
  fPoFile    = NULL;
  fLastErrno = 0;

  fFileSize  = -1;

  fOutFile   = -1;
  fOutGzFile = NULL;

  //int endian = 0x12345678;
  //fDoByteSwap = (*(char)(&endian) != 0x78);
}

static int hasSuffix(const char *name,const char *suffix) {
  const char *s = strstr(name,suffix);
  if(s==NULL)
    return 0;
  return (s-name)+strlen(suffix) == strlen(name);
}

bool TGEBFile::Open(const char *filename) {
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
    fprintf(stderr,"TGEBFile::Open: Reading from pipe: %s\n",pipe.c_str());
    fPoFile = popen(pipe.c_str(),"r");

    if(fPoFile == NULL) {
      fLastErrno = errno;
      fLastError = strerror(errno);
      return false;
    }

    fFile = fileno((FILE*)fPoFile);
  } else {
     fFile = open(filename, O_RDONLY | O_LARGEFILE);
     
     if(fFile<=0) { 
       fLastErrno = errno;
       fLastError = strerror(errno);
       return false;
     }

     if(hasSuffix(filename,".gz")) {
       fGzFile = new gzFile;
       (*(gzFile*)fGzFile) = gzdopen(fFile,"rb");

       if((*(gzFile*)fGzFile)==NULL) {
         fLastErrno = -1;
         fLastError = "zlib gzdopen() error";
         return false;
       }
     }
  }
  return true;
}

bool TGEBFile::OpenOut(const char *filename) {

  if(fOutFile>0)
    CloseOut();

  fOutFilename = filename;
  fOutFile = open(filename,O_WRONLY | O_CREAT | O_TRUNC | O_LARGEFILE, 0644);

  if(fOutFile <= 0) {
    fLastErrno = errno;
    fLastError = strerror(errno);
    return false;
  }

  if(hasSuffix(filename,".gz")) {
    fOutGzFile = new gzFile;
    *(gzFile*)fOutGzFile = gzdopen(fOutFile,"wb");
    if((*(gzFile*)fOutGzFile) == NULL) {
      fLastErrno = -1;
      fLastError = "zlib gzdopen() error";
      return false;
    }
    if(gzsetparams(*(gzFile*)fOutGzFile,1,Z_DEFAULT_STRATEGY) != Z_OK) {
      fLastErrno = -1;
      fLastError = "zlib gzsetparams() error";
      return false;
    }

  }
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

int TGEBFile::Read(TGEBEvent *gebevent) {

   if(!gebevent) {
    fprintf(stderr,"TGEBFILE::Read;  Trying to filee NULL TGEBEvent.");
    return -1;
   }  

   gebevent->Clear();
   int rd = 0;
   int rd_head = 0;
   if(fGzFile) 
     rd_head = gzread(*(gzFile*)fGzFile,(char*)gebevent->GetEventHeader(),sizeof(TGEBEventHeader));
   else 
     rd_head = readpipe(fFile,(char*)gebevent->GetEventHeader(),sizeof(TGEBEventHeader));

   if(rd_head==0) {
     fLastErrno = 0;
     fLastError = "EOF";
     return 0;
   } else if (rd_head != sizeof(TGEBEventHeader)) {
     fLastErrno = errno;
     fLastError = strerror(errno);
     return -1;
   }
   
   if(fDoByteSwap) {
     //do byte swap.
   }

   if(!gebevent->IsGoodSize()) {
     fLastErrno = -1;
     fLastError = "Invalid event size";
     return -1;
   }

   if(fGzFile) 
    rd = gzread(*(gzFile*)fGzFile,gebevent->GetData(),gebevent->GetDataSize());
   else 
     rd = readpipe(fFile,gebevent->GetData(),gebevent->GetDataSize());

   if(rd != (int)gebevent->GetDataSize()) {
     fLastErrno = errno;
     fLastError = strerror(errno);
     return -1;
   }


  return rd + rd_head;
}

int TGEBFile::Write(TGEBEvent *gebevent) {
  int wr = -2;
  if(fOutGzFile)
    wr = gzwrite(*(gzFile*)fOutGzFile,(char*)gebevent->GetEventHeader(),sizeof(TGEBEventHeader));
  else
    wr = write(fOutFile,(char*)gebevent->GetEventHeader(),sizeof(TGEBEventHeader));

  if(wr != sizeof(TGEBEventHeader)) {
    fprintf(stderr,"TGEBFile: write header error; return %i, size requested %lu\n",wr,sizeof(TGEBEventHeader));
    return -1;
  }

  if(fOutGzFile)
    wr = gzwrite(*(gzFile*)fOutGzFile,(char*)gebevent->GetData(),gebevent->GetDataSize());
  else 
    wr = write(fOutFile,(char*)gebevent->GetData(),gebevent->GetDataSize());

  
  if(wr != gebevent->GetDataSize()) {
    fprintf(stderr,"TGEBFile: write data error; return %i, size requested %lu\n",wr,gebevent->GetDataSize());
    return -1;
  }

  return wr;
}

void TGEBFile::Close() {
  if(fPoFile)
    pclose((FILE*)fPoFile);
  fPoFile = NULL;

  if(fGzFile)
    gzclose(*(gzFile*)fGzFile);   
  fGzFile = NULL;
  
  if(fFile>0) 
    close(fFile);
  fFile = -1;
  fFilename = "";
}

void TGEBFile::CloseOut() {
  if(fOutGzFile) {
    gzflush(*(gzFile*)fOutGzFile,Z_FULL_FLUSH);
    gzclose(*(gzFile*)fOutGzFile);
  }
  fOutGzFile = NULL;
  if(fOutFile>0)
    close(fOutFile);
  fOutFile = -1;
  fOutFilename = "";
}

int TGEBFile::GetRunNumber() {  }

void TGEBFile::Clear(Option_t *opt) { } 

void TGEBFile::Print(Option_t *opt) { } 

size_t TGEBFile::FindFileSize(const char* fname) {
  ifstream temp;
  temp.open(fname, std::ios::in | std::ios::ate);
  size_t fsize = temp.tellg();
  temp.close();
  return fsize;
}

size_t TGEBFile::GetFileSize() {
  if(fFileSize==-1)
    fFileSize = FindFileSize(GetFileName());
  return fFileSize;
}



