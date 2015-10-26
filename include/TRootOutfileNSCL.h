#ifndef _TROOTOUTFILENSCL_H_
#define _TROOTOUTFILENSCL_H_

#include "TRootOutfile.h"

class TJanus;
class TSega;

class TRootOutfileNSCL : public TRootOutfile {
public:
  TRootOutfileNSCL();
  ~TRootOutfileNSCL();

  virtual void Clear(Option_t* option = "");
  virtual void Print(Option_t* option = "") const;

private:
  virtual void Init();

  TJanus*    janus;
  TSega*     sega;

  // TS800*     s800;
  // TCaesar*   caesar;

  //TTree* event_tree;
  //TTree* scaler_tree;

  ClassDef(TRootOutfileNSCL, 0);
};

#endif /* _TROOTOUTFILE_H_ */
