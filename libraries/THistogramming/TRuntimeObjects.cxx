#include "TRuntimeObjects.h"

#include <iostream>

#include "TClass.h"
#include "TCutG.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TDirectoryFile.h"
#include "TProfile.h"

#include "TH1D.h"
#include "TH2D.h"

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
    hist = new TH1D(name,name,bins,low,high);
    GetObjects().Add(hist);
  }
  if(!(std::isnan(value))) {
    hist->Fill(value, weight);
  }
  return hist;
}

TH1* TRuntimeObjects::FillHistogram(const char* name,
                                    int bins, double low, double high, const char *value,
                                    double weight){
  TH1* hist = (TH1*) GetObjects().FindObject(name);
  if(!hist){
    hist = new TH1D(name,name,bins,low,high);
    GetObjects().Add(hist);
  }
  if(value!=0) {
    hist->Fill(value, weight);
  }
  return hist;
}

TH2* TRuntimeObjects::FillHistogram(const char* name,
                                    int Xbins, double Xlow, double Xhigh, double Xvalue,
                                    int Ybins, double Ylow, double Yhigh, double Yvalue,
                                    double weight){
  TH2* hist = (TH2*) GetObjects().FindObject(name);
  if(!hist){
    hist = new TH2D(name,name,
                    Xbins, Xlow, Xhigh,
                    Ybins, Ylow, Yhigh);
    GetObjects().Add(hist);
  }
  if(!std::isnan(Xvalue) && !std::isnan(Yvalue)) {
    hist->Fill(Xvalue, Yvalue, weight);
  }
  return hist;
}

TH2* TRuntimeObjects::FillHistogram(const char* name,
                                    int Xbins, double Xlow, double Xhigh, const char *Xvalue,
                                    int Ybins, double Ylow, double Yhigh, double Yvalue,
                                    double weight){
  TH2* hist = (TH2*) GetObjects().FindObject(name);
  if(!hist){
    hist = new TH2D(name,name,
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

TH2* TRuntimeObjects::FillHistogramSym(const char* name,
                                    int Xbins, double Xlow, double Xhigh, double Xvalue,
                                    int Ybins, double Ylow, double Yhigh, double Yvalue){
  TH2* hist = (TH2*) GetObjects().FindObject(name);
  if(!hist){
    hist = new TH2D(name,name,
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
  TH1* hist = (TH1*)dir->FindObject(name);
  if(!hist){
    hist = new TH1D(name,name,
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
  TH1* hist = (TH1*)dir->FindObject(name);
  if(!hist){
    hist = new TH1D(name,name,
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
  TH2* hist = (TH2*)dir->FindObject(name);
  if(!hist){
    hist = new TH2D(name,name,
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
  TH2* hist = (TH2*)dir->FindObject(name);
  if(!hist){
    hist = new TH2D(name,name,
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
  TH2* hist = (TH2*)dir->FindObject(name);
  if(!hist){
    hist = new TH2D(name,name,
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
  return dir;
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
