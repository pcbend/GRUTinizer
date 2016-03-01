#include "TRuntimeObjects.h"

#include <iostream>

#include "TClass.h"
#include "TH1.h"
#include "TH2.h"
#include "TDirectoryFile.h"

#include "GH1D.h"
#include "GH2D.h"
#include "GH2D.h"

#include "GValue.h"


std::map<std::string,TRuntimeObjects*> TRuntimeObjects::fRuntimeMap;

TRuntimeObjects::TRuntimeObjects(TUnpackedEvent *detectors, TList* objects, TList* variables, TList *gates,
                                 TDirectory* directory,const char *name)
  : detectors(detectors), objects(objects), variables(variables), gates(gates), directory(directory) {
  SetName(name);
  fRuntimeMap.insert(std::make_pair(name,this));
}

TRuntimeObjects::TRuntimeObjects(TList* objects, TList* variables, TList *gates,
                                 TDirectory* directory,const char *name)
  : detectors(0),objects(objects), variables(variables), gates(gates), directory(directory) {
  SetName(name);
  fRuntimeMap.insert(std::make_pair(name,this));
}


TH1* TRuntimeObjects::FillHistogram(const char* name,
                                    int bins, double low, double high, double value){
  TH1* hist = (TH1*) GetObjects().FindObject(name);
  if(!hist){
    hist = new TH1I(name,name,bins,low,high);
    GetObjects().Add(hist);
  }
  hist->Fill(value);
  return hist;
}

TH2* TRuntimeObjects::FillHistogram(const char* name,
                                    int Xbins, double Xlow, double Xhigh, double Xvalue,
                                    int Ybins, double Ylow, double Yhigh, double Yvalue){
  TH2* hist = (TH2*) GetObjects().FindObject(name);
  if(!hist){
    hist = new GH2D(name,name,
                            Xbins, Xlow, Xhigh,
                            Ybins, Ylow, Yhigh);
    GetObjects().Add(hist);
  }
  hist->Fill(Xvalue, Yvalue);
  return hist;
}

TH2* TRuntimeObjects::FillHistogramSym(const char* name,
                                    int Xbins, double Xlow, double Xhigh, double Xvalue,
                                    int Ybins, double Ylow, double Yhigh, double Yvalue){
  TH2* hist = (TH2*) GetObjects().FindObject(name);
  if(!hist){
    hist = new GH2D(name,name,
                            Xbins, Xlow, Xhigh,
                            Ybins, Ylow, Yhigh);
    GetObjects().Add(hist);
  }
  hist->Fill(Xvalue, Yvalue);
  hist->Fill(Yvalue, Xvalue);
  return hist;
}

//-------------------------------------------------------------------------
TDirectory* TRuntimeObjects::FillHistogram(const char* dirname,const char* name,
					   int bins, double low, double high, double value){

  TDirectory *dir = (TDirectory*)GetObjects().FindObject(dirname);
  if(!dir){
    dir = new TDirectory(dirname,dirname);
    GetObjects().Add(dir);
  }
  dir->cd();
  TH1* hist = (TH1*)dir->FindObject(name);
  if(!hist){
    hist = new TH1I(name,name,
		    bins, low, high);
    dir->Add(hist);
  }

  hist->Fill(value);
  dir->cd("../");
  //return hist;
  return dir;

  /*
  std::cout << "1" << std::endl;
  if(!(gDirectory->cd(dirname))){
      gDirectory->mkdir(dirname);
        std::cout << "2" << std::endl;
  }
  std::cout << "3" << std::endl;
  TDirectory *dir = gDirectory->GetDirectory(dirname);
  //gDirectory->pwd()
  std::cout << "4" << std::endl;;
  TH1* hist = (TH1*)dir->FindObject(name);
  std::cout << "4a" << std::endl;
  if(!hist){
    std::cout << "4b" << std::endl;
    hist = new TH1I(name,name,bins,low,high);
    //    GetObjects().Add(hist);
    std::cout << "5" << std::endl;
    dir->Add(hist);
    std::cout << "6" << std::endl;
  }
  std::cout << "7" << std::endl;
  hist->Fill(value);
  std::cout << "8" << std::endl;
  gDirectory->cd("../");
  std::cout << "9" << std::endl;
  return dir;
  //return hist;*/
}

TDirectory* TRuntimeObjects::FillHistogram(const char* dirname,const char* name,
                                    int Xbins, double Xlow, double Xhigh, double Xvalue,
                                    int Ybins, double Ylow, double Yhigh, double Yvalue){
  TDirectory *dir = (TDirectory*)GetObjects().FindObject(dirname);
  if(!dir){
    dir = new TDirectory(dirname,dirname);
    GetObjects().Add(dir);
  }
  dir->cd();
  TH2* hist = (TH2*)dir->FindObject(name);
  if(!hist){
    hist = new GH2D(name,name,
                            Xbins, Xlow, Xhigh,
                            Ybins, Ylow, Yhigh);
    dir->Add(hist);
  }

  hist->Fill(Xvalue, Yvalue);
  dir->cd("../");
  //return hist;
  return dir;/*
  TH2* hist = (TH2*) GetObjects().FindObject(name);
  if(!hist){
    hist = new GH2D(name.c_str(),name.c_str(),
                            Xbins, Xlow, Xhigh,
                            Ybins, Ylow, Yhigh);
    GetObjects().Add(hist);
  }
  hist->Fill(Xvalue, Yvalue);
  return hist;*/
}

TDirectory* TRuntimeObjects::FillHistogramSym(const char* dirname,const char* name,
                                    int Xbins, double Xlow, double Xhigh, double Xvalue,
                                    int Ybins, double Ylow, double Yhigh, double Yvalue){
  TDirectory *dir = (TDirectory*)GetObjects().FindObject(dirname);
  if(!dir){
    dir = new TDirectory(dirname,dirname);
    GetObjects().Add(dir);
  }
  dir->cd();
  TH2* hist = (TH2*)dir->FindObject(name);
  if(!hist){
    hist = new GH2D(name,name,
                            Xbins, Xlow, Xhigh,
                            Ybins, Ylow, Yhigh);
    dir->Add(hist);
  }
  hist->Fill(Xvalue, Yvalue);
  hist->Fill(Yvalue, Xvalue);
  dir->cd("../");
  //return hist;
  return dir;/*
  TH2* hist = (TH2*) GetObjects().FindObject(name);
  if(!hist){
    hist = new GH2D(name.c_str(),name.c_str(),
                            Xbins, Xlow, Xhigh,
                            Ybins, Ylow, Yhigh);
    GetObjects().Add(hist);
  }
  hist->Fill(Xvalue, Yvalue);
  return hist;*/
}
//-------------------------------------------------------------------------

TList& TRuntimeObjects::GetObjects() {
  return *objects;
}

TList& TRuntimeObjects::GetGates() {
  return *gates;
}

TList& TRuntimeObjects::GetVariables() {
  return *variables;
}

TCutG* TRuntimeObjects::GetCut(const std::string& name) {
  /*
  TIter next(cuts);
  TObject* obj;
  while((obj = next())){
    TCutG* cut = dynamic_cast<TCutG*>(obj);
    if(cut &&
       obj->GetName() == name){
      return (TCutG*)obj;
    }
  }
  */
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
