
#include <GCutG.h>

#include <TFile.h>
#include <TEnv.h>
#include <TPreserveGDirectory.h>

#include <string>

ClassImp(GCutG)


void GCutG::Print(Option_t *opt) const {
  printf("%s[%s] print function\n",IsA()->GetName(),GetName());

}

int GCutG::SaveTo(const char *cutname,const char* filename,Option_t* option) { 
  TPreserveGDirectory pd;
  std::string fname = filename;
  std::string cname = cutname;
  if(!fname.length())
    cname = this->GetName();
  if(cname.find_first_of("_")==0)
    cname = cname.substr(1);
  if(!fname.length())
    fname = gEnv->GetValue("GRUT.DefaultCutFile","");
  GCutG *cut = (GCutG*)this->Clone(cname.c_str());
  TFile f(fname.c_str(),option);
  return cut->Write(cname.c_str(),TObject::kOverwrite);
}
