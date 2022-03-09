
#include <TString.h>
#include <TOldSega.h>
#include <TOldSegaHit.h>



void TOldSegaHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
  segments.clear();
}

void TOldSegaHit::Copy(TObject &rhs) const {
  TDetectorHit::Copy(rhs);
  ((TOldSegaHit&)rhs).segments = this->segments;
}

void TOldSegaHit::Print(Option_t *opt) const {
  TString sopt(opt);


  printf("det[%02i]\tchg[0x%08x]\ttime[0x%08x]i\n",
          GetDetId(),Charge(),Time()); 
  if(sopt.Contains("all")) {
    for(int i=0;i<segments.size();i++) {
      printf("\tseg[%02i]:  %08x\n",GetSegId(i),GetSegChg(i));
    }
  }
}

void TOldSegaHit::AddSegment(int id, float charge) {
  TDetectorHit hit;
  unsigned int address = 0x5e6a0000;
  address+= (GetDetId()<<8);
  address+= id;

  hit.SetAddress(address);
  hit.SetCharge(charge);
  hit.SetTime(-1);
  hit.SetTimestamp(-1);
  segments.push_back(hit);

}


TVector3 TOldSegaHit::GetPosition() const {
  
  return TOldSega::GetGlobalSegmentPosition(GetDetNum(),GetSegId());

}


