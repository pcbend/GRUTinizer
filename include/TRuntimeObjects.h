#ifndef _RUNTIMEOBJECTS_H_
#define _RUNTIMEOBJECTS_H_

#include <string>
#include <map>

#include "TCutG.h"
#include "TDirectory.h"
#include "TList.h"

#include "TUnpackedEvent.h"

class TH1;
class TH2;

/// Object passed to the online histograms.
/**
   For each event, an instance of this type will be passed to the custom histogrammer.
   This class contains all detectors present, and all existing cuts and histograms.
 */
class TRuntimeObjects : public TNamed {
public:
  /// Constructor
  TRuntimeObjects(TUnpackedEvent& detectors,
                  TList* objects,
                  TList* variables,
                  TList* gates,
                  TDirectory* directory=NULL,
                  const char *name="default");

  /// Returns a pointer to the detector of type T
  template<typename T>
  T* GetDetector(){
    return detectors.GetDetector<T>();
  }

  TCutG* GetCut(const std::string& name);

  TList& GetObjects();
  TList& GetGates();
  TList& GetVariables();

  TList* GetObjectsPtr();
  TList* GetGatesPtr();
  TList* GetVariablesPtr();


  TH1* FillHistogram(std::string name,
                     int bins, double low, double high, double value);
  TH2* FillHistogram(std::string name,
                     int Xbins, double Xlow, double Xhigh, double Xvalue,
                     int Ybins, double Ylow, double Yhigh, double Yvalue);
  TH2* FillHistogramSym(std::string name,
                     int Xbins, double Xlow, double Xhigh, double Xvalue,
                     int Ybins, double Ylow, double Yhigh, double Yvalue);

  double GetVariable(const char* name);

  static TRuntimeObjects *Get(std::string name="default") { if(fRuntimeMap.count(name)) return fRuntimeMap.at(name); return 0; }

private:
  static std::map<std::string,TRuntimeObjects*> fRuntimeMap;
  TUnpackedEvent& detectors;
  TList* objects;
  TList* variables;
  TList* gates;
  TDirectory* directory;


  ClassDef(TRuntimeObjects, 0);
};

#endif /* _RUNTIMEOBJECTS_H_ */
