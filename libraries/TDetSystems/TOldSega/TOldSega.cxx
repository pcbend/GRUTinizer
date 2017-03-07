
#include <TOldSega.h>


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


