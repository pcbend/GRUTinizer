
#include <GCutG.h>

#include <TFile.h>
#include <TEnv.h>
#include <TPreserveGDirectory.h>
#include <TClass.h>
#include <TMethodCall.h>


#include <string>

ClassImp(GCutG)


void GCutG::Print(Option_t *opt) const {
  printf("%s[%s] print function\n",IsA()->GetName(),GetName());

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
  return cut->Write(cname.c_str(),TObject::kOverwrite);
}


void GCutG::SetGateMethod(const char* xclass,const char* xmethod,
                          const char* yclass,const char* ymethod) {
  fXGateClass   = xclass; 
  fYGateClass   = yclass;  
  fXGateMethod  = xmethod; 
  fYGateMethod  = ymethod; 

}

bool GCutG::IsInside(TObject *objx,TObject *objy) {
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
  return TCutG::IsInside(storagex,storagey);
  
}

