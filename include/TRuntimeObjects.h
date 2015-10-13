#ifndef _RUNTIMEOBJECTS_H_
#define _RUNTIMEOBJECTS_H_

#include <string>

#include "TCutG.h"
#include "TList.h"

class TRuntimeObjects : public TObject {
public:
  TRuntimeObjects(TList* detectors, TList* objects);

  TList& GetDetectors();
  TList& GetObjects();

  void FillHistogram(std::string name,
                     int bins, double low, double high, double value);
  void FillHistogram(std::string name,
                     int Xbins, double Xlow, double Xhigh, double Xvalue,
                     int Ybins, double Ylow, double Yhigh, double Yvalue);

private:
  TList* detectors;
  TList* objects;


  ClassDef(TRuntimeObjects, 0);
};

#endif /* _RUNTIMEOBJECTS_H_ */
