#include "GH2D.h"

#include <iostream>

#include <TDirectory.h>
#include <TClass.h>
#include <TMethodCall.h>

#include "GH1D.h"
#include "GCutG.h"
#include "TRuntimeObjects.h"

ClassImp(GH2D)

GH2D::GH2D(const char *name,const char *title,Int_t nbinsx,const Double_t *xbins,Int_t nbinsy, const Double_t *ybins) :
  TH2D(name,title,nbinsx,xbins,nbinsy,ybins), GH2Base(),
  fXFillClass(0),fYFillClass(0),fXFillMethod(0),fYFillMethod(0) {
}


GH2D::GH2D(const char *name,const char *title,Int_t nbinsx,const Float_t *xbins,Int_t nbinsy, const Float_t *ybins) :
  TH2D(name,title,nbinsx,xbins,nbinsy,ybins), GH2Base(),
  fXFillClass(0),fYFillClass(0),fXFillMethod(0),fYFillMethod(0) {
}


GH2D::GH2D(const char *name,const char *title,Int_t nbinsx,const Double_t *xbins,
                                            Int_t nbinsy, Double_t ylow, Double_t yup) :
  TH2D(name,title,nbinsx,xbins,nbinsy,ylow,yup), GH2Base(),
  fXFillClass(0),fYFillClass(0),fXFillMethod(0),fYFillMethod(0) {
}


GH2D::GH2D(const char *name,const char *title,Int_t nbinsx, Double_t xlow, Double_t xup,
                                            Int_t nbinsy, Double_t *ybins) :
  TH2D(name,title,nbinsx,xlow,xup,nbinsy,ybins), GH2Base(),
  fXFillClass(0),fYFillClass(0),fXFillMethod(0),fYFillMethod(0) {
}


GH2D::GH2D(const char *name,const char *title,Int_t nbinsx, Double_t xlow, Double_t xup,
                                            Int_t nbinsy, Double_t ylow, Double_t yup) :
  TH2D(name,title,nbinsx,xlow,xup,nbinsy,ylow,yup), GH2Base(),
  fXFillClass(0),fYFillClass(0),fXFillMethod(0),fYFillMethod(0) {
}

GH2D::GH2D(const TObject &obj) {
  if(obj.InheritsFrom(TH2::Class())){
    obj.Copy(*this);
  }
}

GH2D::~GH2D() {  }


void GH2D::Copy(TObject &obj) const {
  TH2::Copy(obj);
  GH2D& g = (GH2D&)obj; 
  g.fXFillClass = fXFillClass;
  g.fYFillClass = fYFillClass;
  g.fXFillMethod = fXFillMethod;
  g.fYFillMethod = fYFillMethod;
 //fProjections->Copy(*(((GH2D&)obj).fProjections));
  //fSummaryProjections->Copy(*(((GH2D&)obj).fSummaryProjections));
}

TObject *GH2D::Clone(const char *newname) const {
  std::string name = newname;
  if(!name.length())
    name = Form("%s_clone",GetName());
  return TH2::Clone(name.c_str());
}

void GH2D::Clear(Option_t *opt) {
  TString sopt(opt);
  if(!sopt.Contains("projonly")){
    TH2D::Clear(opt);
  }
  GH2Clear();
}

void GH2D::Print(Option_t *opt) const { }

void GH2D::Draw(Option_t *opt) {
  std::string option = opt;
  if(option == ""){
    option = "colz";
  }
  TH2D::Draw(option.c_str());
  if(gPad) {
    gPad->Update();
    gPad->GetFrame()->SetBit(TBox::kCannotMove);
  }
}


//void GH2D::Draw(TCutG *cut) {
//  if(!cut)
//    return;
//  std::string option = Form("colz [%s]",cut->GetName());
//  TH2D::Draw(option.c_str());
//}



TH1 *GH2D::DrawCopy(Option_t *opt) const {
  TH1 *h = TH2D::DrawCopy(opt);
  if(gPad) {
    gPad->Update();
    gPad->GetFrame()->SetBit(TBox::kCannotMove);
  }
  return h;
}

TH1 *GH2D::DrawNormalized(Option_t *opt,Double_t norm) const {
  TH1 *h = TH2D::DrawNormalized(opt,norm);
  if(gPad) {
    gPad->Update();
    gPad->GetFrame()->SetBit(TBox::kCannotMove);
  }
  return h;
}



GH1D* GH2D::ProjectionX(const char* name,
                       int firstbin,
                       int lastbin,
                       Option_t* option) {
  return GH2ProjectionX(name,firstbin,lastbin,option);
}



GH1D* GH2D::ProjectionY(const char* name,
                       int firstbin,
                       int lastbin,
                       Option_t* option) {
  return GH2ProjectionY(name,firstbin,lastbin,option);
}

/*
void GH2D::Streamer(TBuffer &b) {
  if(b.IsReading()) {
    Version_t v = b.ReadVersion(); 
    TH2D::Streamer(b);
    TDirectory *current = gDirectory;
    if(TDirectory::Cd(Form("%s_projections",this->GetName()))) { 
      TList *list = gDirectory->GetList();
      TIter iter(list);
      while(TObject *obj = iter.Next()) {
        if(obj->InheritsFrom(TH1::Class())) {
          GH1D *h = new GH1D(*obj);
          h->SetParent(this);
          fProjections.Add(h);
        }  
      }
    }
    current->cd();
  } else {
    b.WriteVersion(GH2D::IsA());
    TH2D::Streamer(b);
    if(fProjections.GetEntries()) {
      TDirectory *current = gDirectory;
      TDirectory *newdir  =  current->mkdir(Form("%s_projections",this->GetName());
      newdir->cd();
      fProjections->Write();
      current->cd();
    }

  }
}
*/


void GH2D::SetFillMethod(const char *classnamex,const char *methodnamex,const char *paramx,
                         const char *classnamey,const char *methodnamey,const char *paramy) {
  fXFillClass = TClass::GetClass(classnamex);
  if(!fXFillClass)
    return;
  fYFillClass = TClass::GetClass(classnamey);
  if(!fYFillClass)
    return;
  fXFillMethod = new TMethodCall(fXFillClass,methodnamex,paramx);
  fYFillMethod = new TMethodCall(fYFillClass,methodnamey,paramy);
  //printf("class:  %s\n",fFillClass->GetName()); 
  //printf("method: %s\n",fFillMethod->GetMethod()->GetPrototype()); 
}


Int_t GH2D::Fill(const TObject* objx,const TObject *objy) {
  if(!fXFillClass || !fXFillMethod || !fYFillClass || !fYFillMethod) {
    //printf("%p \t %p\n",fFillClass,fFillMethod);
    return -1;
  }
  if(!objy) {
    objy=objx;
  }
  if(objx->IsA()!=fXFillClass || objy->IsA()!=fYFillClass ) {
    //printf("%s \t %s\n", obj->Class()->GetName(),fFillClass->GetName());
    return -2;
  }
  //for(auto gate : gates) {
  //  if(!gate->IsInside())
  //    return -3;
  //}
  Double_t storagex;
  Double_t storagey;
  fXFillMethod->Execute((void*)(objx),storagex);
  fYFillMethod->Execute((void*)(objy),storagey);
  return TH2D::Fill(storagex,storagey);
}

Int_t GH2D::Fill(const TRuntimeObjects *objs) {
  if(!fXFillClass || !fXFillMethod || !fYFillClass || !fYFillMethod) {
    //printf("%p \t %p\n",fFillClass,fFillMethod);
    return -1;
  }
  //for(auto gate : gates) {
  //  if(!gate->IsInside(objs))
  //    return -3;
  //}
  TDetector *detx = objs->GetDetector(fXFillClass->GetName());
  TDetector *dety = objs->GetDetector(fYFillClass->GetName());
  //printf("detx = %s\n",detx->IsA()->GetName()); fflush(stdout);
  //printf("dety = %s\n",dety->IsA()->GetName()); fflush(stdout);

  return Fill(detx,dety);
}



void GH2D::RemoveGate(GCutG *gate) {
  unsigned int i=0;
  for(auto g : gates) {
    if(g==gate)
      break;
    i++;
  };
  if(i<gates.size()) {
    gates.erase(gates.begin()+i);
  }
}


