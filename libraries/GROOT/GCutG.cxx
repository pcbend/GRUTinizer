
#include <GCutG.h>

#include <TFile.h>
#include <TEnv.h>
#include <TPreserveGDirectory.h>
#include <TClass.h>
#include <TMethodCall.h>

#include <string>

#include <TRuntimeObjects.h>

ClassImp(GCutG)


void GCutG::Print(Option_t *opt) const {
  printf("%s[%s] print function\n",IsA()->GetName(),GetName());

}

void GCutG::Init(double low,double high) {
  fXlow  = low;
  fXhigh = high;
}

int GCutG::SaveTo(const char *cutname,const char* filename,Option_t* option) { 
  TPreserveGDirectory pd;
  std::string fname = filename;
  std::string cname = cutname;
  if(!cname.length())
    cname = this->GetName();
  if(cname.find_first_of("_")==0)
    cname = cname.substr(1);
  if(!fname.length())
    fname = gEnv->GetValue("GRUT.DefaultCutFile","");
  GCutG *cut = (GCutG*)this->Clone(cname.c_str());
  cut->SetNameTitle(cname.c_str(),cname.c_str());
  TFile f(fname.c_str(),option);
  //printf("Attepting to save %s to %s... ",cut->GetName(),f.GetName());
  if(f.Get(cname.c_str())) {
    f.cd();//Get(cname.c_str())->Delete();
    gDirectory->Delete(cname.c_str());
  }
  //int bytes = cut->Write(cname.c_str(),TObject::kOverwrite | TObject::kSingleKey);
  f.Add(cut,true);
  //printf("%i\n",bytes);  fflush(stdout);
  return f.Write();
}


void GCutG::SetGateMethod(const char* xclass,const char* xmethod,
                          const char* yclass,const char* ymethod,
                          const char* xparam,const char* yparam) {
  fXGateClass   = xclass; 
  fYGateClass   = yclass;  
  fXGateMethod  = xmethod; 
  fYGateMethod  = ymethod; 
  fXGateParam   = xparam; 
  fYGateParam   = yparam; 

}

Int_t GCutG::IsInside(TObject *objx,TObject *objy) const {
  if(!objy)
    objy = objx;

  TClass *xclass = TClass::GetClass(fXGateClass.Data());
  TClass *yclass = TClass::GetClass(fYGateClass.Data());

  TMethodCall *xmethod = new TMethodCall(xclass,fXGateMethod.Data(),"");
  TMethodCall *ymethod = new TMethodCall(yclass,fYGateMethod.Data(),"");
  Double_t storagex;
  Double_t storagey;
  xmethod->Execute((void*)(objx),storagex);
  ymethod->Execute((void*)(objy),storagey);
  return IsInside(storagex,storagey);

}

Int_t GCutG::IsInside(Double_t x,Double_t y) const {
  switch(Dimension()) {
    case 1:
      return x>fXlow&&x<fXhigh;
    case 2:
      return TCutG::IsInside(x,y); 
    default:
      return 0; // Arrgah.  it is not clear what the default, "i am not setup" should be.
  };
}

Int_t GCutG::IsInside(const TRuntimeObjects *objs) const {

  if(fXGateClass.IsNull() || fXGateMethod.IsNull() ) {
    //printf("%p \t %p\n",fFillClass,fFillMethod);
    return -1;
  }  
 
  TDetector *detx = objs->GetDetector(fXGateClass.Data());
  TDetector *dety = 0;
  if(fYGateClass.IsNull() || fYGateMethod.IsNull()) {
    dety = objs->GetDetector(fYGateClass.Data());
  }
  return IsInside(detx,dety);
  //return 0;
}



















void GCutG::RemoveAllPoints() {
  for(int i=GetN()-1;i<=0;i--) 
    RemovePoint(i);
}



