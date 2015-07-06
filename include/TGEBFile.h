#ifndef TGEBFILE_H
#define TGEBFILE_H

//#include "Globals.h"

#include <TObject.h>

//#include "TGEBBanks.h"


class TGEBEvent;


class TGEBFile : public TObject {

  public:
    TGEBFile();
    TGEBFile(const char *fname);
    virtual ~TGEBFile();

    bool Open(const char *fname);
    bool OpenOut(const char *fname);

    void Close();
    void CloseOut();

    int Read(TGEBEvent*); 
    int Write(TGEBEvent*);

    const char *GetFileName()  const { return fFilename.c_str();  }
    int         GetLastErrno() const { return fLastErrno;         }
    const char *GetLastError() const { return fLastError.c_str(); }
  
    void Print(Option_t *opt = "");
    void Clear(Option_t *opt = "");

    size_t GetFileSize(); 
    static size_t FindFileSize(const char*);
    int GetRunNumber();

  private:
    std::string fFilename;
    std::string fOutFilename;

    void InitTGEBFile();

    int         fLastErrno;
    std::string fLastError;

    bool fDoByteSwap;

    size_t fFileSize;
    size_t fBytesRead;
    size_t fBytesWritten;

    Int_t  fExperimentNumber;
    Int_t  fRunNumber;

   
    int    fFile;
    void*  fGzFile;
    void*  fPoFile;
    int    fOutFile;
    void*  fOutGzFile;


  ClassDef(TGEBFile,0)
};

#endif

