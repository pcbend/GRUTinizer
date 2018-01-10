#include "TRuntimeObjects.h"

#include <iostream>

#include "TClass.h"
#include "TCutG.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TDirectoryFile.h"
#include "TProfile.h"

#include "GH1D.h"
#include "GH2D.h"
#include "GH2D.h"

#include "GValue.h"






std::map<std::string,TRuntimeObjects*> TRuntimeObjects::fRuntimeMap;

TRuntimeObjects::TRuntimeObjects(TUnpackedEvent *detectors, TList* objects, TList *gates,
                                 std::vector<TFile*>& cut_files,
                                 TDirectory* directory,const char *name)
  : detectors(detectors), objects(objects), gates(gates),
    cut_files(cut_files),
    directory(directory) {
  SetName(name);
  fRuntimeMap.insert(std::make_pair(name,this));
}

TRuntimeObjects::TRuntimeObjects(TList* objects, TList *gates,
                                 std::vector<TFile*>& cut_files,
                                 TDirectory* directory,const char *name)
  : detectors(0),objects(objects), gates(gates),
    cut_files(cut_files),
    directory(directory) {
  SetName(name);
  fRuntimeMap.insert(std::make_pair(name,this));
}


TH1* TRuntimeObjects::FillHistogram(const char* name,
                                    int bins, double low, double high, double value,
                                    double weight){
  TH1* hist = (TH1*) GetObjects().FindObject(name);
  if(!hist){
    //std::cout << "New Histogram: " << name << std::endl;
    hist = new GH1D(name,name,bins,low,high);
    GetObjects().Add(hist);
  }
  if(!(std::isnan(value))) {
    hist->Fill(value, weight);
  }
  return hist;
}

GH1D* TRuntimeObjects::InitHistogram(const char* name,const char *title,int bins, double low, double high) {
  GH1D* hist = (GH1D*) GetObjects().FindObject(name);
  if(!hist){
    //std::cout << "New Histogram: " << name << std::endl;
    hist = new GH1D(name,title,bins,low,high);
    GetObjects().Add(hist);
  }
  return hist;
}

TH2* TRuntimeObjects::FillHistogram(const char* name,
                                    int Xbins, double Xlow, double Xhigh, double Xvalue,
                                    int Ybins, double Ylow, double Yhigh, double Yvalue,
                                    double weight){
  TH2* hist = (TH2*) GetObjects().FindObject(name);
  if(!hist){
    //std::cout << "New Histogram: " << name << std::endl;
    hist = new GH2D(name,name,
                    Xbins, Xlow, Xhigh,
                    Ybins, Ylow, Yhigh);
    GetObjects().Add(hist);
  }
  if(!std::isnan(Xvalue) && !std::isnan(Yvalue)) {
    hist->Fill(Xvalue, Yvalue, weight);
  }
  return hist;
}

GH2D* TRuntimeObjects::InitHistogram(const char* name,const char *title,int Xbins, double Xlow, double Xhigh,
                                                                        int Ybins, double Ylow, double Yhigh) {
  GH2D* hist = (GH2D*) GetObjects().FindObject(name);
  if(!hist){
    //std::cout << "New Histogram: " << name << std::endl;
    hist = new GH2D(name,title,
                    Xbins, Xlow, Xhigh,
                    Ybins, Ylow, Yhigh);
    GetObjects().Add(hist);
  }
  return hist;
}

int  TRuntimeObjects::FillHistogram(const char* name) {
  TH1* hist = (TH1*) GetObjects().FindObject(name);
  if(!hist){
    return -100;
  }
  if(hist->GetDimension()==1) {
    GH1D *ghist = (GH1D*)hist;
    return ghist->Fill(this);
  } else {
    GH2D *ghist = (GH2D*)hist;
    return ghist->Fill(this);
  }
}


TProfile* TRuntimeObjects::FillProfileHist(const char* name,
					   int Xbins, double Xlow, double Xhigh, double Xvalue,
					   double Yvalue){
  TProfile* prof = (TProfile*)GetObjects().FindObject(name);
  if(!prof){
    //std::cout << "New Histogram: " << name << std::endl;
    prof = new TProfile(name,name,
			Xbins,Xlow,Xhigh);
    GetObjects().Add(prof);
  }
  if(!(std::isnan(Xvalue)))
    if(!(std::isnan(Yvalue)))
      prof->Fill(Xvalue,Yvalue);
  return prof;
}

TH2* TRuntimeObjects::FillHistogramSym(const char* name,
                                    int Xbins, double Xlow, double Xhigh, double Xvalue,
                                    int Ybins, double Ylow, double Yhigh, double Yvalue){
  TH2* hist = (TH2*) GetObjects().FindObject(name);
  if(!hist){
    //std::cout << "New Histogram: " << name << std::endl;
    hist = new GH2D(name,name,
                            Xbins, Xlow, Xhigh,
                            Ybins, Ylow, Yhigh);
    GetObjects().Add(hist);
  }

  if(!(std::isnan(Xvalue))){
    if(!(std::isnan(Yvalue))){
      hist->Fill(Xvalue, Yvalue);
      hist->Fill(Yvalue, Xvalue);
    }
  }
  return hist;
}

//-------------------------------------------------------------------------
TDirectory* TRuntimeObjects::FillHistogram(const char* dirname,const char* name,
					   int bins, double low, double high, double value,
                                           double weight){


  TDirectory *dir = (TDirectory*)GetObjects().FindObject(dirname);
  if(!dir){
    dir = new TDirectory(dirname,dirname);
    GetObjects().Add(dir);
  }
  TH1* hist = (TH1*)dir->FindObject(name);
  if(!hist){
    dir->cd();
    //std::cout << "New Histogram: " << name << std::endl;
    hist = new GH1D(name,name,
		    bins, low, high);
    dir->Add(hist);
    dir->cd("../");
  }

  if(!std::isnan(value)) {
    hist->Fill(value, weight);
  }
  //return hist;
  return dir;

}

GH1D* TRuntimeObjects::InitHistogram(const char* dirname,const char* name,const char *title,
                                   int bins, double low, double high) {
  TDirectory *dir = (TDirectory*)GetObjects().FindObject(dirname);
  if(!dir){
    dir = new TDirectory(dirname,dirname);
    GetObjects().Add(dir);
  }

  GH1D* hist = (GH1D*)dir->FindObject(name);
  if(!hist){
    dir->cd();
    //std::cout << "New Histogram: " << name << std::endl;
    hist = new GH1D(name,title,
		    bins, low, high);
    dir->Add(hist);
    dir->cd("../");
  }

  return hist;
}

int TRuntimeObjects::FillHistogram(const char* dirname,const char* name) {
  TDirectory *dir = (TDirectory*)GetObjects().FindObject(dirname);
  if(!dir){
    return -2000;
  }

  TH1* hist = (TH1*)dir->FindObject(name);
  if(!hist){
    return -1000;
  }
  if(hist->GetDimension()==1) {
    GH1D *ghist = (GH1D*)hist;
    return ghist->Fill(this);
  } else {
    GH2D *ghist = (GH2D*)hist;
    return ghist->Fill(this);
  }
}


TDirectory* TRuntimeObjects::FillHistogram(const char* dirname,const char* name,
                                           int Xbins, double Xlow, double Xhigh, double Xvalue,
                                           int Ybins, double Ylow, double Yhigh, double Yvalue,
                                           double weight){
  TDirectory *dir = (TDirectory*)GetObjects().FindObject(dirname);
  if(!dir){
    dir = new TDirectory(dirname,dirname);
    GetObjects().Add(dir);
  }

  TH2* hist = (TH2*)dir->FindObject(name);
  if(!hist){
    dir->cd();
    //std::cout << "New Histogram: " << name << std::endl;
    hist = new GH2D(name,name,
                            Xbins, Xlow, Xhigh,
                            Ybins, Ylow, Yhigh);
    dir->Add(hist);
    dir->cd("../");
  }

  if(!std::isnan(Xvalue) && !std::isnan(Yvalue)) {
    hist->Fill(Xvalue, Yvalue, weight);
  }

  //return hist;
  return dir;
}

GH2D* TRuntimeObjects::InitHistogram(const char* dirname,const char* name,const char* title,
                                           int Xbins, double Xlow, double Xhigh,
                                           int Ybins, double Ylow, double Yhigh) {
  TDirectory *dir = (TDirectory*)GetObjects().FindObject(dirname);
  if(!dir){
    dir = new TDirectory(dirname,dirname);
    GetObjects().Add(dir);
  }

  GH2D* hist = (GH2D*)dir->FindObject(name);
  if(!hist){
    dir->cd();
    //std::cout << "New Histogram: " << name << std::endl;
    hist = new GH2D(name,name,
                            Xbins, Xlow, Xhigh,
                            Ybins, Ylow, Yhigh);
    dir->Add(hist);
    dir->cd("../");
  }

  //return hist;
  return hist;
}


TDirectory* TRuntimeObjects::FillProfileHist(const char* dirname,const char* name,
					     int Xbins, double Xlow, double Xhigh, double Xvalue,
					     double Yvalue){

  TDirectory *dir = (TDirectory*)GetObjects().FindObject(dirname);
  if(!dir){
    dir = new TDirectory(dirname,dirname);
    GetObjects().Add(dir);
  }

  TProfile *prof = (TProfile*)dir->FindObject(name);
  if(!prof){
    dir->cd();
    //std::cout << "New Histogram: " << name << std::endl;
    prof = new TProfile(name,name,
			Xbins,Xlow,Xhigh);
   dir->Add(prof);
   dir->cd("../");
  }

  if(!(std::isnan(Xvalue)))
    if(!(std::isnan(Yvalue)))
      prof->Fill(Xvalue,Yvalue);

  return dir;
}

TDirectory* TRuntimeObjects::FillHistogramSym(const char* dirname,const char* name,
                                    int Xbins, double Xlow, double Xhigh, double Xvalue,
                                    int Ybins, double Ylow, double Yhigh, double Yvalue){
  TDirectory *dir = (TDirectory*)GetObjects().FindObject(dirname);
  if(!dir){
    dir = new TDirectory(dirname,dirname);
    GetObjects().Add(dir);
  }

  TH2* hist = (TH2*)dir->FindObject(name);
  if(!hist){
    dir->cd();
    //std::cout << "New Histogram: " << name << std::endl;
    hist = new GH2D(name,name,
                            Xbins, Xlow, Xhigh,
                            Ybins, Ylow, Yhigh);
    dir->Add(hist);
    dir->cd("../");
  }
  if(!(std::isnan(Xvalue))){
    if(!(std::isnan(Yvalue))){
      hist->Fill(Xvalue, Yvalue);
      hist->Fill(Yvalue, Xvalue);
    }
  }

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

TCutG* TRuntimeObjects::GetCut(const std::string& name) {
  for(auto& tfile : cut_files) {
    TObject* obj = tfile->Get(name.c_str());
    if(obj) {
      TCutG* cut = dynamic_cast<TCutG*>(obj);
      if(cut) {
        return cut;
      }
    }
  }
  return NULL;
}

double TRuntimeObjects::GetVariable(const char* name) {
  return GValue::Value(name);
}
