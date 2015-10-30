#ifndef _RUNTIMEOBJECTS_H_
#define _RUNTIMEOBJECTS_H_

#include <string>

#include "TCutG.h"
#include "TList.h"

#include "TUnpackedEvent.h"

/// Object passed to the online histograms.
/**
   For each event, an instance of this type will be passed to the custom histogrammer.
   This class contains all detectors present, and all existing cuts and histograms.
 */
class TRuntimeObjects : public TObject {
public:
  /// Constructor
  TRuntimeObjects(TUnpackedEvent& detectors,
                  TList* objects,
                  TList* variables);

  /// Returns a pointer to the detector of type T
  template<typename T>
  T* GetDetector(){
    return detectors.GetDetector<T>();
  }

  TCutG* GetCut(const std::string& name);

  TList& GetObjects();
  TList& GetVariables();

  void FillHistogram(std::string name,
                     int bins, double low, double high, double value);
  void FillHistogram(std::string name,
                     int Xbins, double Xlow, double Xhigh, double Xvalue,
                     int Ybins, double Ylow, double Yhigh, double Yvalue);

  double GetVariable(const char* name);

private:
  TUnpackedEvent& detectors;
  TList* objects;
  TList* variables;


  ClassDef(TRuntimeObjects, 0);
};

#endif /* _RUNTIMEOBJECTS_H_ */
