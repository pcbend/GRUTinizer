
#include <TOldSega.h>

bool TOldSega::fPositionsSet = false;
TVector3 TOldSega::fSegaPositions[30]; 

void TOldSega::Clear(Option_t *opt) {
  sega_hits.clear();

  masterlive = -1;
  xfpscint   = -1;
  rf         = -1;

}

void TOldSega::Copy(TObject &rhs) const {

  TDetector::Copy(rhs);
  ((TOldSega&)rhs).masterlive = this->masterlive;
  ((TOldSega&)rhs).xfpscint   = this->xfpscint;
  ((TOldSega&)rhs).rf         = this->rf;
  
  ((TOldSega&)rhs).sega_hits = this->sega_hits;


}

void TOldSega::Print(Option_t *opt) const {
  
  printf("-----------------------\n");
  printf("TOldSega with %i hits:\n",Size());
  printf("  MasterLive: %i\n",masterlive);
  printf("  XfpScint:   %i\n",xfpscint);
  printf("  Rf:         %i\n",rf);
  for(int i=0;i<sega_hits.size();i++) {
    printf("\t"); 
    sega_hits.at(i).Print(opt);
  }
  printf("-----------------------\n");

}


void TOldSega::SetPositions() {
  if(fPositionsSet==true) return;


  fSegaPositions[0].SetMagThetaPhi(1,0,0);
  fSegaPositions[1].SetMagThetaPhi(1,0,0);
  fSegaPositions[2].SetMagThetaPhi(23,TMath::DegToRad()*32.1,TMath::DegToRad()*45.);
  fSegaPositions[3].SetMagThetaPhi(23,TMath::DegToRad()*32.1,TMath::DegToRad()*90.);
  fSegaPositions[4].SetMagThetaPhi(23,TMath::DegToRad()*32.1,TMath::DegToRad()*135.);
  fSegaPositions[5].SetMagThetaPhi(23,TMath::DegToRad()*32.1,TMath::DegToRad()*180.);
  fSegaPositions[6].SetMagThetaPhi(23,TMath::DegToRad()*32.1,TMath::DegToRad()*225.);
  fSegaPositions[7].SetMagThetaPhi(1,0,0);
  fSegaPositions[8].SetMagThetaPhi(1,0,0);
  fSegaPositions[9].SetMagThetaPhi(23,TMath::DegToRad()*32.1,TMath::DegToRad()*270.);
  fSegaPositions[10].SetMagThetaPhi(23,TMath::DegToRad()*32.1,TMath::DegToRad()*315.);
  fSegaPositions[11].SetMagThetaPhi(23,TMath::DegToRad()*84.8,TMath::DegToRad()*0.);
  fSegaPositions[12].SetMagThetaPhi(23,TMath::DegToRad()*84.8,TMath::DegToRad()*36.);
  fSegaPositions[13].SetMagThetaPhi(23,TMath::DegToRad()*84.8,TMath::DegToRad()*72.);
  fSegaPositions[14].SetMagThetaPhi(23,TMath::DegToRad()*84.8,TMath::DegToRad()*108.);
  fSegaPositions[15].SetMagThetaPhi(1,0,0);
  fSegaPositions[16].SetMagThetaPhi(1,0,0);
  fSegaPositions[17].SetMagThetaPhi(23,TMath::DegToRad()*84.8,TMath::DegToRad()*144.);
  fSegaPositions[18].SetMagThetaPhi(23,TMath::DegToRad()*84.8,TMath::DegToRad()*180.);
  fSegaPositions[19].SetMagThetaPhi(23,TMath::DegToRad()*84.8,TMath::DegToRad()*216.);
  fSegaPositions[20].SetMagThetaPhi(23,TMath::DegToRad()*84.8,TMath::DegToRad()*252.);
  fSegaPositions[21].SetMagThetaPhi(23,TMath::DegToRad()*84.8,TMath::DegToRad()*288.);
  fSegaPositions[22].SetMagThetaPhi(23,TMath::DegToRad()*84.8,TMath::DegToRad()*324.);
  fSegaPositions[23].SetMagThetaPhi(1,0,0);
  fSegaPositions[24].SetMagThetaPhi(1,0,0);
  fSegaPositions[25].SetMagThetaPhi(1,0,0);
  fSegaPositions[26].SetMagThetaPhi(1,0,0);
  fSegaPositions[27].SetMagThetaPhi(1,0,0);
  fSegaPositions[28].SetMagThetaPhi(1,0,0);
  fSegaPositions[29].SetMagThetaPhi(1,0,0);
  fPositionsSet = true;
}

















