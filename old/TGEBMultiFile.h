#ifndef TGEBMULTIFILE_H
#define TGEBMULTIFILE_H

//#include "Globals.h"

#include <TObject.h>

//#include "TGEBBanks.h"
#include <vector>
//#include <map>


#include <TGEBEvent.h>
#include <TGEBFile.h>

class TGEBMultiFile : public TObject {

  public:
    TGEBMultiFile();
    virtual ~TGEBMultiFile();

    bool Add(const char *fname);
    bool Add(TGEBFile *file);
    
    void InitiMultFiles();

    void Close();

    int Read(TGEBEvent*); 

    const char *GetLastFileName()  const { return fFileList.at(fLastFileNumber).c_str();  }
    int         GetLastErrno()     const { return fLastErrno;         }
    const char *GetLastError()     const { return fLastError.c_str(); }
 
    void ShowStatus(Option_t *opt = "") const; 
    void Print(Option_t *opt = "") const;
    void Clear(Option_t *opt = "");

    //size_t GetFileSize(); 
    //static size_t FindFileSize(const char*);
    //int GetRunNumber();

    
  private:
    
    std::vector<std::string> fFileList;
 
    int         fLastFileNumber;
    int         fLastErrno;
    std::string fLastError;

    //size_t fFileSize;
    //size_t fBytesRead;
    //size_t fBytesWritten;

    Int_t  fExperimentNumber;
    Int_t  fRunNumber;

    std::vector<TGEBEvent*> fEvents;
    std::vector<TGEBFile*>  fFiles;
    std::vector<int>        fEventCounter;
    std::vector<Long_t>     fLastTimeStamp;

    std::vector<size_t>     fBytes;
    std::vector<size_t>     fBytesRead;

    

    inline TGEBEvent *GetEvent(const int &i) { return fEvents.at(i); } 
    inline TGEBFile  *GetFile(const int &i)  { return fFiles.at(i);  } 
    inline Long_t     GetTime(const int &i)  { return fEvents.at(i)->GetTimeStamp(); }
    
    bool GetNextEvent(int);     
    int FindMinimum();


  ClassDef(TGEBMultiFile,0)
};

#endif

