#ifndef _TROOTOUTFILENSCL_H_
#define _TROOTOUTFILENSCL_H_

#include "TRootOutfile.h"
//#include "TGRUTTypes.h"
//#include "TDetector.h"
//#include "TRawEvent.h"

class TJanus;
class TSega;
class TFastScint;

class TRootOutfileNSCL : public TRootOutfile {
  public:
    TRootOutfileNSCL();
    ~TRootOutfileNSCL();

    virtual void Init(const char* output_filename);
    virtual void InitHists();
    virtual void FillHists();
   
    virtual void Clear(Option_t* option = "");
    virtual void Print(Option_t* option = "") const;
   
  private:
    TJanus*    janus;
    TSega*     sega;
 
    // TS800*     s800;
    // TCaesar*   caesar;

    TFastScint* fastscint;

    //TTree* event_tree;
    //TTree* scaler_tree;

  ClassDef(TRootOutfileNSCL, 0);
};

#endif /* _TROOTOUTFILE_H_ */
