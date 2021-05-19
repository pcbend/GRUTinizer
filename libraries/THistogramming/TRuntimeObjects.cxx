#include "TRuntimeObjects.h"

#include <iostream>

#include "TClass.h"
#include "TCutG.h"
#include "TFile.h"
#include "GH1.h"
#include "GH2.h"
#include "TDirectoryFile.h"
#include "TProfile.h"

#include "GH1D.h"
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


GH1* TRuntimeObjects::FillHistogram(const char* name,
                                    int bins, double low, double high, double value,
                                    double weight){
  GH1* hist = (GH1*) GetObjects().FindObject(name);
  if(!hist){
    hist = new GH1D(name,name,bins,low,high);
    GetObjects().Add(hist);
  }
  if(!(std::isnan(value))) {
    hist->Fill(value, weight);
  }
  return hist;
}

GH1* TRuntimeObjects::FillHistogram(const char* name,
                                    int bins, double low, double high, const char *value,
                                    double weight){
  GH1* hist = (GH1*) GetObjects().FindObject(name);
  if(!hist){
    hist = new GH1D(name,name,bins,low,high);
    GetObjects().Add(hist);
  }
  if(value!=0) {
    hist->Fill(value, weight);
  }
  return hist;
}

GH2* TRuntimeObjects::FillHistogram(const char* name,
                                    int Xbins, double Xlow, double Xhigh, double Xvalue,
                                    int Ybins, double Ylow, double Yhigh, double Yvalue,
                                    double weight){
  GH2* hist = (GH2*) GetObjects().FindObject(name);
  if(!hist){
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

GH2* TRuntimeObjects::FillHistogram(const char* name,
                                    int Xbins, double Xlow, double Xhigh, const char *Xvalue,
                                    int Ybins, double Ylow, double Yhigh, double Yvalue,
                                    double weight){
  GH2* hist = (GH2*) GetObjects().FindObject(name);
  if(!hist){
    hist = new GH2D(name,name,
                    Xbins, Xlow, Xhigh,
                    Ybins, Ylow, Yhigh);
    GetObjects().Add(hist);
  }
  if(Xvalue!=0 && !std::isnan(Yvalue)) {
    hist->Fill(Xvalue, Yvalue, weight);
  }
  return hist;
}

TProfile* TRuntimeObjects::FillProfileHist(const char* name,
					   int Xbins, double Xlow, double Xhigh, double Xvalue,
					   double Yvalue){
  TProfile* prof = (TProfile*)GetObjects().FindObject(name);
  if(!prof){
    prof = new TProfile(name,name,
			Xbins,Xlow,Xhigh);
    GetObjects().Add(prof);
  }
  if(!(std::isnan(Xvalue)))
    if(!(std::isnan(Yvalue)))
      prof->Fill(Xvalue,Yvalue);
  return prof;
}

GH2* TRuntimeObjects::FillHistogramSym(const char* name,
                                    int Xbins, double Xlow, double Xhigh, double Xvalue,
                                    int Ybins, double Ylow, double Yhigh, double Yvalue){
  GH2* hist = (GH2*) GetObjects().FindObject(name);
  if(!hist){
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
  dir->cd();
  GH1* hist = (GH1*)dir->FindObject(name);
  if(!hist){
    hist = new GH1D(name,name,
		    bins, low, high);
    hist->SetDirectory(dir); // dir->Add(hist);
  }

  if(!std::isnan(value)) {
    hist->Fill(value, weight);
  }
  dir->cd("../");
  //return hist;
  return dir;

}
TDirectory* TRuntimeObjects::FillHistogram(const char* dirname,const char* name,
					   int bins, double low, double high, const char *value,
                                           double weight){
  TDirectory *dir = (TDirectory*)GetObjects().FindObject(dirname);
  if(!dir){
    dir = new TDirectory(dirname,dirname);
    GetObjects().Add(dir);
  }
  dir->cd();
  GH1* hist = (GH1*)dir->FindObject(name);
  if(!hist){
    hist = new GH1D(name,name,
		    bins, low, high);
    hist->SetDirectory(dir); // dir->Add(hist);
  }

  if(value) {
    hist->Fill(value, weight);
  }
  dir->cd("../");
  //return hist;
  return dir;

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
  dir->cd();
  GH2* hist = (GH2*)dir->FindObject(name);
  if(!hist){
    hist = new GH2D(name,name,
                            Xbins, Xlow, Xhigh,
                            Ybins, Ylow, Yhigh);
    hist->SetDirectory(dir); // dir->Add(hist);
  }

  if(!std::isnan(Xvalue) && !std::isnan(Yvalue)) {
    hist->Fill(Xvalue, Yvalue, weight);
  }
  dir->cd("../");
  return dir;
}
TDirectory* TRuntimeObjects::FillHistogram(const char* dirname,const char* name,
                                           int Xbins, double Xlow, double Xhigh, const char *Xvalue,
                                           int Ybins, double Ylow, double Yhigh, double Yvalue,
                                           double weight){
  TDirectory *dir = (TDirectory*)GetObjects().FindObject(dirname);
  if(!dir){
    dir = new TDirectory(dirname,dirname);
    GetObjects().Add(dir);
  }
  dir->cd();
  GH2* hist = (GH2*)dir->FindObject(name);
  if(!hist){
    hist = new GH2D(name,name,
                            Xbins, Xlow, Xhigh,
                            Ybins, Ylow, Yhigh);
    hist->SetDirectory(dir); // dir->Add(hist);
  }

  if(Xvalue && !std::isnan(Yvalue)) {
    hist->Fill(Xvalue, Yvalue, weight);
  }
  dir->cd("../");
  return dir;
}


TDirectory* TRuntimeObjects::FillProfileHist(const char* dirname,const char* name,
					     int Xbins, double Xlow, double Xhigh, double Xvalue,
					     double Yvalue){

  TDirectory *dir = (TDirectory*)GetObjects().FindObject(dirname);
  if(!dir){
    dir = new TDirectory(dirname,dirname);
    GetObjects().Add(dir);
  }
  dir->cd();
  TProfile *prof = (TProfile*)dir->FindObject(name);
  if(!prof){
    prof = new TProfile(name,name,
			Xbins,Xlow,Xhigh);
   dir->Add(prof);
  }

  if(!(std::isnan(Xvalue)))
    if(!(std::isnan(Yvalue)))
      prof->Fill(Xvalue,Yvalue);
  dir->cd("../");
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
  dir->cd();
  GH2* hist = (GH2*)dir->FindObject(name);
  if(!hist){
    hist = new GH2D(name,name,
                            Xbins, Xlow, Xhigh,
                            Ybins, Ylow, Yhigh);
    hist->SetDirectory(dir); // dir->Add(hist);
  }
  if(!(std::isnan(Xvalue))){
    if(!(std::isnan(Yvalue))){
      hist->Fill(Xvalue, Yvalue);
      hist->Fill(Yvalue, Xvalue);
    }
  }
  dir->cd("../");
  //return hist;
  return dir;/*
  GH2* hist = (GH2*) GetObjects().FindObject(name);
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
