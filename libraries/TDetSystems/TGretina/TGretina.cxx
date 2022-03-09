#include <deque>
#include <fstream>
#include <string>
#include <sstream>
#include <set>


#include "TGretina.h"
//#include "GRootCommands.h"
//#include <TPad.h>
//#include <TROOT.h>

//#include "GH1D.h"
//#include "GH2I.h"
//#include "TGRUTOptions.h"
//#include "GCanvas.h"

#include "TGEBEvent.h"

TGretina::TGretina(){
  //gretina_hits = new TClonesArray("TGretinaHit");
  //addback_hits = new TClonesArray("TGretinaHit");
  Clear();
}

TGretina::~TGretina() {
  //delete gretina_hits;
  //addback_hits->Delete();
}

Float_t TGretina::crmat[32][4][4][4];
Float_t TGretina::m_segpos[2][36][3];
bool    TGretina::fCRMATSet = false;

bool DefaultAddback(const TGretinaHit& one,const TGretinaHit &two) {
  TVector3 res = one.GetLastPosition()-two.GetPosition();
  return ((std::abs(one.GetTime()-two.GetTime()) < 44.0) &&
          (res.Mag() < 80.0) ) ;
}

std::function<bool(const TGretinaHit&,const TGretinaHit&)> TGretina::fAddbackCondition = DefaultAddback;

void TGretina::BuildAddback(int EngRange) const {
  if( addback_hits.size() > 0 ||
      gretina_hits.size() == 0) {
    return;
  }

  addback_hits = gretina_hits;

  if(EngRange>=0 && EngRange<4){
    for(auto& hit : addback_hits) {
      hit.SetCoreEnergy(hit.GetCoreEnergy(EngRange));
    }
  }

  
  std::sort(addback_hits.begin(), addback_hits.end(),
	    [](const TGretinaHit& a, const TGretinaHit& b) {
	      return a.GetCoreEnergy() > b.GetCoreEnergy();
	    });

  for(unsigned int i=0; i<addback_hits.size(); i++) {
    TGretinaHit& current_hit = addback_hits[i];
    std::vector<unsigned int> to_erase;
    for(unsigned int j=i+1; j<addback_hits.size(); j++) {
      TGretinaHit& other_hit = addback_hits[j];
      if(fAddbackCondition(current_hit, other_hit)) {
	current_hit.Add(other_hit);
	to_erase.push_back(j);
      }
    }

    for(auto it = to_erase.rbegin(); it<to_erase.rend(); it++) {
      unsigned int erasing = *it;
      addback_hits.erase(addback_hits.begin() + erasing);
    }
  }
}

void TGretina::SetCRMAT() {
  if(fCRMATSet){
    return;
  }

  FILE *fp;
  std::string temp = getenv("GRUTSYS");
  temp.append("/libraries/TDetSystems/TGretina/crmat.dat");
  const char *fn = temp.c_str();
  float f1, f2, f3, f4;
  int pos, xtal;
  int nn = 0;
  char *st, str[256];
  fp = fopen64(fn, "r");
  if (fp == NULL) {
    printf("Could not open \"%s\".\n", fn);
    exit(1);
  }
  //printf("\"%s\" open....", fn);
  /* Read values. */
  nn = 0;
  st = fgets(str, 256, fp);
  while (st != NULL) {
    if (str[0] == 35) {
      /* '#' comment line, do nothing */
    } else if (str[0] == 59) {
      /* ';' comment line, do nothing */
    } else if (str[0] == 10) {
      /* Empty line, do nothing */
    } else {
      sscanf(str, "%i %i", &pos, &xtal);
      for(int i=0; i<4; i++) {
        st = fgets(str, 256, fp);
        sscanf(str, "%f %f %f %f", &f1, &f2, &f3, &f4);
        crmat[pos-1][xtal][i][0] = f1;
        crmat[pos-1][xtal][i][1] = f2;
        crmat[pos-1][xtal][i][2] = f3;
        crmat[pos-1][xtal][i][3] = f4;
      }
      nn++;
    }
    /* Attempt to read the next line. */
    st = fgets(str, 256, fp);
  }
  SetSegmentCRMAT();
  fCRMATSet = true;
  //printf("Read %i rotation matrix coefficients.\n", nn);
  /* Done! */
}

void TGretina::SetSegmentCRMAT() {
  if(fCRMATSet)
    return;
  //FILE *infile;
  int NUMSEG = 36;
  std::string temp = getenv("GRUTSYS");
  temp.append("/libraries/TDetSystems/TGretina/crmat_segpos.dat");
  //infile = fopen(temp.c_str(),"r");
  std::ifstream infile;
  infile.open(temp.c_str());
  if(!infile.is_open()) {
    return;
  }
  // notice: In file type-A is first, then 
  //         type-B but as type-B are even
  //         det_ids in the data stream we
  //         define type=0 for type-B not
  //         type-A.
  //
  //printf("filename: %s\n",temp.c_str());
  std::string line;
  int type = 0;
  int seg  = 0;
  while(getline(infile,line)) {
    if(seg==NUMSEG) {
      seg  = 0;
      type = 1;
    }
    //printf("%s\n",line.c_str());
    std::stringstream ss(line);
    int segread;
    double x,y,z;
    ss >> segread >> x >> y >> z;
    if((seg+1)!=segread) {
      fprintf(stderr,"%s: seg[%i] read but seg[%i] expected.\n",__PRETTY_FUNCTION__,segread,seg+1);
    }
    //printf("type[%i] : seg[%02i] : %.02f  %.02f  %.02f\n",type,seg,x,y,z);
    m_segpos[(type+1)%2][seg][0] = x;
    m_segpos[(type+1)%2][seg][1] = y;
    m_segpos[(type+1)%2][seg][2] = z;
    //m_segpos[(type)%2][seg][0] = x;
    //m_segpos[(type)%2][seg][1] = y;
    //m_segpos[(type)%2][seg][2] = z;
    seg++;
  }
  
  infile.close();
  //fclose(infile);
}


TVector3 TGretina::GetSegmentPosition(int cry_id,int segment) {
  SetCRMAT();
  float x = m_segpos[cry_id%2][segment][0];
  float y = m_segpos[cry_id%2][segment][1];
  float z = m_segpos[cry_id%2][segment][2];
  TVector3 v(x,y,z);
  if((cry_id%2)==0) {
    if(cry_id==96 || cry_id==98) {
    } else {
      v.RotateZ(-TMath::Pi()/3.);
    }
    //v.RotateX(TMath::Pi());
  } else {
    //v.RotateY(TMath::Pi());
  }
  //v.RotateX(TMath::Pi());
  //v.RotateY(TMath::Pi());
  //return CrystalToGlobal(cry_id,x,y,z);
  return CrystalToGlobal(cry_id,v.X(),v.Y(),v.Z());
}

TVector3 TGretina::GetCrystalPosition(int cry_id) {
  SetCRMAT();
  //return CrystalToGlobal(cry_id,0.0,0.0,0.0); 
  
  TVector3 v;
  v.SetXYZ(0.0,0.0,0.0);
  for(int i=30;i<36;i++) {
    TVector3 a = GetSegmentPosition(cry_id,i);
    v.SetXYZ(v.X()+a.X(),v.Y()+a.Y(),v.Z()+a.Z());
  }
  v.SetXYZ(v.X()/6.0,v.Y()/6.0,v.Z()/6.0);
  return v;      
  
}

void TGretina::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TGretina& gretina = (TGretina&)obj;
  gretina.gretina_hits = gretina_hits; // gretina_hits->Copy(*gretina.gretina_hits);
  //addback_hits->Copy(*gretina.addback_hits);
}

void TGretina::InsertHit(const TDetectorHit& hit){
  //TGretinaHit* new_hit = (TGretinaHit*)gretina_hits->ConstructedAt(Size());
  //hit.Copy(*new_hit);
  gretina_hits.emplace_back((TGretinaHit&)hit);
  fSize++;
}

int TGretina::BuildHits(std::vector<TRawEvent>& raw_data){
  //printf("%s\n",__PRETTY_FUNCTION__);
  if(raw_data.size()<1)
    return Size();
  long smallest_time = 0x3fffffffffffffff;
  for(auto& event : raw_data){
    if(event.GetTimestamp()<smallest_time)
      smallest_time=event.GetTimestamp();
    // TGEBEvent* geb = (TGEBEvent*)&event;
    // const TRawEvent::GEBBankType1* raw = (const TRawEvent::GEBBankType1*)geb->GetPayloadBuffer().GetData();
    TGretinaHit hit;
    TSmartBuffer buf = event.GetPayloadBuffer();
    hit.BuildFrom(buf);
    InsertHit(hit);
  }
  SetTimestamp(smallest_time);
  //gretina_hits->At(0)->Print();
  //BuildAddbackHits();

  //gretina_hits->At(0)->Print();
  return Size();
}

TVector3 TGretina::CrystalToGlobal(int cryId,Float_t x,Float_t y,Float_t z) {
  SetCRMAT();

  Int_t detectorPosition = cryId/4 - 1;
  Int_t crystalNumber    = cryId%4;

  /* x,y,z need to be in cm to work properly. Depending on the
     source of the mapping, you might need to convert from mm
     (if you read from crmat.linux). */
  double xl = ( (crmat[detectorPosition][crystalNumber][0][0] * x) +
                (crmat[detectorPosition][crystalNumber][0][1] * y) +
                (crmat[detectorPosition][crystalNumber][0][2] * z) +
                (crmat[detectorPosition][crystalNumber][0][3]) );

  double yl = ( (crmat[detectorPosition][crystalNumber][1][0] * x) +
                (crmat[detectorPosition][crystalNumber][1][1] * y) +
                (crmat[detectorPosition][crystalNumber][1][2] * z) +
                (crmat[detectorPosition][crystalNumber][1][3]) );

  double zl = ( (crmat[detectorPosition][crystalNumber][2][0] * x) +
                (crmat[detectorPosition][crystalNumber][2][1] * y) +
                (crmat[detectorPosition][crystalNumber][2][2] * z) +
                (crmat[detectorPosition][crystalNumber][2][3]) );

  return TVector3(xl, yl, zl);
}

/*
void TGretina::BuildAddbackHits(){
  if(Size()==0)
    return;

  addback_hits->Clear();
  TGretinaHit *newhit = (TGretinaHit*)addback_hits->ConstructedAt(0);
  GetHit(0).Copy(*newhit);

  if(Size()==1)
    return;

  std::vector<double> max_energies;
  max_energies.push_back(newhit->GetCoreEnergy());

  for(int x=1;x<Size();x++) {

    bool used = false;
    for(int y=0;y<addback_hits->GetEntries();y++) {
      if(GetAddbackHit(y).CheckAddback(GetGretinaHit(x))) {
        used = true;
        GetAddbackHit(y).Add(GetGretinaHit(x), max_energies[y]);
        break;
      }
    }

    if(!used) {
      TGretinaHit *tmphit = (TGretinaHit*)addback_hits->ConstructedAt(addback_hits->GetEntries());
      GetGretinaHit(x).Copy(*tmphit);
      max_energies.push_back(newhit->GetCoreEnergy());
    }
  }
}
*/

void TGretina::Print(Option_t *opt) const {
  printf(BLUE "GRETINA: size = %i" RESET_COLOR "\n",(int)Size());
  for(unsigned int x=0;x<Size();x++) {
    printf(DYELLOW);
    GetGretinaHit(x).Print(opt);
    printf(RESET_COLOR);
  }
  printf(BLUE "--------------------------------" RESET_COLOR "\n");
}

void TGretina::SortHits() {
  std::sort(gretina_hits.begin(),gretina_hits.end());
}

void TGretina::Clear(Option_t *opt) {
  TDetector::Clear(opt);
  gretina_hits.clear();
  addback_hits.clear();
}

/*
void TGretina::DrawDopplerGamma(Double_t Beta,Option_t *gate,Option_t *opt,Long_t nentries,TChain *chain){
  TString OptString = opt;
  if(!chain)
    chain = gChain;
  if(!chain || !chain->GetBranch("TGretina"))
    return;
  if(!gPad || !gPad->IsEditable()){
    gROOT->MakeDefCanvas();
  }else{
    gPad->GetCanvas()->Clear();
  }
  std::string name  = Form("%s_Gammas",Class()->GetName());
  std::string title = Form("%s Gamma 1-D",Class()->GetName());
  GH1D *h = (GH1D*)gROOT->FindObject(name.c_str());
  if(!h)
    h = new GH1D(name.c_str(),title.c_str(),4000,0,8000);


  chain->Project(name.c_str(),Form("gretina_hits.GetDoppler(%f)",Beta),gate,opt,nentries);
  h->GetXaxis()->SetTitle("Energy [keV]");
  h->GetYaxis()->SetTitle("Counts / 2 keV");
  h->Draw(opt);

}

void TGretina::DrawDopplerBeta(Option_t *gate,Option_t *opt,Long_t nentries,TChain *chain){
  TString OptString = opt;
  if(!chain)
    chain = gChain;
  if(!chain || !chain->GetBranch("TGretina"))
    return;
  if(!gPad || !gPad->IsEditable()){
    gROOT->MakeDefCanvas();
  }else{
    gPad->GetCanvas()->Clear();
  }
  std::string name  = Form("%s_DopplerBeta",Class()->GetName());
  std::string title = Form("Gretina Doppler Beta");
  GH2I *h = (GH2I*)gROOT->FindObject(name.c_str());
  if(!h)
    h = new GH2I(name.c_str(),title.c_str(),2000,0,4000,101,0.2,0.5);
  double beta = 0.2;
  for(int i = 0; i < 100; i++){
    chain->Project(name.c_str(),Form("gretina_hits.GetDoppler(%f):%f",beta,beta),gate,opt,nentries);
  }
  h->GetXaxis()->SetTitle("Energy [keV]");
  h->GetYaxis()->SetTitle("BETA");
  h->Draw(opt);
  std::cout << " I DONT WORK YET!!!" << std::endl;

}

void TGretina::DrawEnVsTheta(Double_t Beta,Option_t *gate,Option_t *opt,Long_t nentries,TChain *chain){
  TString OptString = opt;
  if(!chain)
    chain = gChain;
  if(!chain || !chain->GetBranch("TGretina"))
    return;
  if(!gPad || !gPad->IsEditable()){
    gROOT->MakeDefCanvas();
  }else{
    gPad->GetCanvas()->Clear();
  }
  std::string name  = Form("%s_En_vs_Theta",Class()->GetName());
  std::string title = Form("Gretina Energy vs Theta");
  GH2I *h = (GH2I*)gROOT->FindObject(name.c_str());
  if(!h)
    h = new GH2I(name.c_str(),title.c_str(),4000,0,4000,314,0,3.14);

  chain->Project(name.c_str(),Form("gretina_hits.GetTheta():gretina_hits.GetDoppler(%f)",Beta),gate,opt,nentries);
  h->GetXaxis()->SetTitle("Theta [radians]");
  h->GetYaxis()->SetTitle("Energy [keV]");
  h->Draw(opt);

}

void TGretina::DrawCoreSummary(Option_t *gate,Option_t *opt,Long_t nentries,TChain *chain){
  TString OptString = opt;
  if(!chain)
    chain = gChain;
  if(!chain || !chain->GetBranch("TGretina"))
    return;
  if(!gPad || !gPad->IsEditable()){
    gROOT->MakeDefCanvas();
  }else{
    gPad->GetCanvas()->Clear();
  }
  std::string name  = Form("%s_Core_Summary",Class()->GetName());
  std::string title = Form("Gretina Core Summary");
  GH2I *h = (GH2I*)gROOT->FindObject(name.c_str());
  if(!h)
    h = new GH2I(name.c_str(),title.c_str(),60,20,80,4000,0,4000);

  chain->Project(name.c_str(),"gretina_hits.GetCoreEnergy():gretina_hits.GetCrystalId()",gate,opt,nentries);
  h->GetXaxis()->SetTitle("Crystal ID");
  h->GetYaxis()->SetTitle("Energy [arb]");
  h->Draw(opt);

}
*/




