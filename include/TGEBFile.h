#ifndef TRawFILE_H
#define TRawFILE_H

//#include "Globals.h"

#include <TObject.h>

//#include "TRawBanks.h"


class TRawEvent;


class TRawFile : public TObject {

  public:
    TRawFile();
    TRawFile(const char *fname);
    virtual ~TRawFile();

    bool Open(const char *fname);
    bool OpenOut(const char *fname);

    void Close();
    void CloseOut();

    int Read(TRawEvent*); 
    int Write(TRawEvent*);

    const char *GetFileName()  const { return fFilename.c_str();  }
    int         GetLastErrno() const { return fLastErrno;         }
    const char *GetLastError() const { return fLastError.c_str(); }
  
    void Print(Option_t *opt = "") const;
    void Clear(Option_t *opt = "");

    size_t GetFileSize(); 
    static size_t FindFileSize(const char*);

  private:
    std::string fFilename;
    std::string fOutFilename;

    void InitTRawFile();

    int         fLastErrno;
    std::string fLastError;

    bool fDoByteSwap;

    size_t fFileSize;
    size_t fBytesRead;
    size_t fBytesWritten;

    int    fFile;
    void*  fGzFile;
    void*  fPoFile;
    int    fOutFile;
    void*  fOutGzFile;


  ClassDef(TRawFile,0)
};

#endif

