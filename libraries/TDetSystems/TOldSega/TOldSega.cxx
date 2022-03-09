
#include <TOldSega.h>
#include <TMath.h>


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
  for(unsigned int i=0;i<sega_hits.size();i++) {
    printf("\t"); 
    sega_hits.at(i).Print(opt);
  }
  printf("-----------------------\n");
}


TVector3 TOldSega::GetGlobalSegmentPosition(int det,int seg) {
  double distance = 230;
  int layer = seg/4;
  int quad  = seg%4; 

  double theta[] = {
    37.0,
    37.0,
    37.0,
    37.0,
    37.0,
    37.0,
    37.0,
    90.0,
    90.0,
    90.0,
    90.0,
    90.0,
    90.0,
    90.0,
    90.0,
    90.0,
    90.0
  };
  double phi[] = {
    45,
    90,
    135,
    180,
    225,
    270,
    315,
    0,
    36,
    72,
    108,
    144,
    180,
    216,
    252,
    288,
    324
  };

  double x,y,z;
  z = distance;

  switch(quad) {
    case 0:
      y = 17.5;
      z+= 17.5;
      break;
    case 1:
      y = -17.5;
      z+= 17.5;
      break;
    case 2:
      y = -17.5;
      z-= 17.5;
      break;
    case 3:
      y = 17.5;
      z-= 17.5;
      break;
  };    
  x = (35.0 - layer*10.0);
  TVector3 vec(x,y,z);
  TVector3 unit(0,0,1);
  unit.SetMagThetaPhi(1,theta[det]*TMath::RadToDeg(),phi[det]*TMath::RadToDeg());
  vec.RotateUz(unit.Unit());
  return vec;
}














