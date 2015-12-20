#include "TRuntimeObjects.h"

#include <iostream>

#include "TClass.h"
#include "TH1.h"
#include "TH2.h"

#include "GValue.h"

TRuntimeObjects::TRuntimeObjects(TUnpackedEvent& detectors, TList* objects, TList* variables,
                                 TDirectory* directory)
  : detectors(detectors), objects(objects), variables(variables), directory(directory) { }

TH1* TRuntimeObjects::FillHistogram(std::string name,
                                    int bins, double low, double high, double value){
  TH1* hist = (TH1*) GetObjects().FindObject(name.c_str());
  if(!hist){
    hist = new TH1I(name.c_str(),name.c_str(),bins,low,high);
    GetObjects().Add(hist);
  }
  hist->Fill(value);
  return hist;
}

TH2* TRuntimeObjects::FillHistogram(std::string name,
                                    int Xbins, double Xlow, double Xhigh, double Xvalue,
                                    int Ybins, double Ylow, double Yhigh, double Yvalue){
  TH2* hist = (TH2*) GetObjects().FindObject(name.c_str());
  if(!hist){
    hist = new TH2I(name.c_str(),name.c_str(),
                            Xbins, Xlow, Xhigh,
                            Ybins, Ylow, Yhigh);
    GetObjects().Add(hist);
  }
  hist->Fill(Xvalue, Yvalue);
  return hist;
}

TH2* TRuntimeObjects::FillHistogramSym(std::string name,
                                    int Xbins, double Xlow, double Xhigh, double Xvalue,
                                    int Ybins, double Ylow, double Yhigh, double Yvalue){
  TH2* hist = (TH2*) GetObjects().FindObject(name.c_str());
  if(!hist){
    hist = new TH2I(name.c_str(),name.c_str(),
                            Xbins, Xlow, Xhigh,
                            Ybins, Ylow, Yhigh);
    GetObjects().Add(hist);
  }
  hist->Fill(Xvalue, Yvalue);
  hist->Fill(Yvalue, Xvalue);
  return hist;
}


TList& TRuntimeObjects::GetObjects() {
  return *objects;
}

TList& TRuntimeObjects::GetVariables() {
  return *variables;
}

TCutG* TRuntimeObjects::GetCut(const std::string& name) {
  TIter next(objects);
  TObject* obj;
  while((obj = next())){
    TCutG* cut = dynamic_cast<TCutG*>(obj);
    if(cut &&
       obj->GetName() == name){
      return (TCutG*)obj;
    }
  }
  return NULL;
}

double TRuntimeObjects::GetVariable(const char* name) {
  TObject* obj = variables->FindObject(name);
  if(obj && dynamic_cast<GValue*>(obj)){
    return ((GValue*)obj)->GetValue();
  } else {
    return 0.00;
    //return std::sqrt(-1);
  }
}
