
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

void GCutG::Clear(Option_t *opt) { TCutG::Clear(opt); }

void GCutG::Copy(const TObject &obj) { 
  if(!obj.InheritsFrom(TCutG::Class()))
    return;

  fVarX    = ((TCutG&)obj).GetVarX();
  fVarY    = ((TCutG&)obj).GetVarY();
  fObjectX = ((TCutG&)obj).GetObjectX();
  fObjectY = ((TCutG&)obj).GetObjectY();

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


//void GCutG::SetGateMethod(const char* xclass,const char* xmethod,
//                          const char* yclass,const char* ymethod) {
//  fXGateClass   = xclass; 
//  fYGateClass   = yclass;  
//  fXGateMethod  = xmethod; 
//  fYGateMethod  = ymethod; 

//}

//bool GCutG::IsInside(TObject *objx,TObject *objy) {
//  if(!objy)
//    objy = objx;
//
//  TClass *xclass = TClass::GetClass(fXGateClass.Data());
//  TClass *yclass = TClass::GetClass(fYGateClass.Data());
//
//  TMethodCall *xmethod = new TMethodCall(xclass,fXGateMethod.Data(),"");
//  TMethodCall *ymethod = new TMethodCall(yclass,fYGateMethod.Data(),"");
//  Double_t storagex;
//  Double_t storagey;
//  xmethod->Execute((void*)(objx),storagex);
//  ymethod->Execute((void*)(objy),storagey);
//  return TCutG::IsInside(storagex,storagey);
//  
//}

Int_t GCutG::IsInside(Double_t x, Double_t y) const {
  //TObject::SetBit(kCutChecked);
  return TCutG::IsInside(x,y);
}

Int_t GCutG::Test(Double_t x, Double_t y) {
  if(Checked())
    return Passed();
  TObject::SetBit(kCutChecked);
  int test = TCutG::IsInside(x,y);
  if(test) TObject::SetBit(kCutPassed);
  return test; 
}


















