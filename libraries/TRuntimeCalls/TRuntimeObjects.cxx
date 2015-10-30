#include "TRuntimeObjects.h"

#include "TH1.h"
#include "TH2.h"

#include "GValue.h"

TRuntimeObjects::TRuntimeObjects(TUnpackedEvent& detectors, TList* objects, TList* variables)
  : detectors(detectors), objects(objects), variables(variables) { }

void TRuntimeObjects::FillHistogram(std::string name,
                                    int bins, double low, double high, double value){
  TH1* hist = (TH1*) GetObjects().FindObject(name.c_str());
  if(hist){
    hist->Fill(value);
  } else {
    TH1* newHist = new TH1I(name.c_str(),name.c_str(),bins,low,high);
    newHist->SetDirectory(0);
    newHist->Fill(value);
    GetObjects().Add(newHist);
  }
}

void TRuntimeObjects::FillHistogram(std::string name,
                                    int Xbins, double Xlow, double Xhigh, double Xvalue,
                                    int Ybins, double Ylow, double Yhigh, double Yvalue){
  TH2* hist = (TH2*) GetObjects().FindObject(name.c_str());
  if(hist){
    hist->Fill(Xvalue, Yvalue);
  } else {
    TH2* newHist = new TH2I(name.c_str(),name.c_str(),
                            Xbins, Xlow, Xhigh,
                            Ybins, Ylow, Yhigh);
    newHist->SetDirectory(0);
    newHist->Fill(Xvalue, Yvalue);
    GetObjects().Add(newHist);
  }
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
    if(obj->InheritsFrom(TCutG::Class()) &&
       obj->GetName() == name){
      return (TCutG*)obj;
    }
  }
  return NULL;
}

double TRuntimeObjects::GetVariable(const char* name) {
  TObject* obj = variables->FindObject(name);
  if(obj && obj->InheritsFrom(GValue::Class())){
    return ((GValue*)obj)->GetValue();
  } else {
    return std::sqrt(-1);
  }
}
