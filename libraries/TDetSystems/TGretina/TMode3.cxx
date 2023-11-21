#include "TMode3.h"
#include "TGEBEvent.h"

/*******************************************************************************/
/* TMode3 **********************************************************************/
/* Contains raw (not decomposed) GRETINA data should not be used for analysis **/
/*******************************************************************************/
TMode3::TMode3(){
  Clear();
}

TMode3::~TMode3() {
}

/*******************************************************************************/
/* Copies hit ******************************************************************/
/*******************************************************************************/
void TMode3::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TMode3& mode3 = (TMode3&)obj;
  mode3.mode3_hits = mode3_hits;
}

/*******************************************************************************/
/* Inserts hit into TMode3 Hit vector ******************************************/
/*******************************************************************************/
void TMode3::InsertHit(const TDetectorHit& hit){
  mode3_hits.emplace_back((TMode3Hit&)hit);
  fSize++;
}

/*******************************************************************************/
/* Unpacks GEB data and builds TMode3 hit **************************************/
/*******************************************************************************/
int TMode3::BuildHits(std::vector<TRawEvent>& raw_data){
  if(raw_data.size()<1)
    return Size();
  long smallest_time = 0x3fffffffffffffff;
  for(auto& event : raw_data){
    if(event.GetTimestamp()<smallest_time)
      smallest_time=event.GetTimestamp();
    TMode3Hit hit;
    TSmartBuffer buf = event.GetPayloadBuffer();
    hit.BuildFrom(buf);
    hit.SetTimestamp(event.GetTimestamp());
    InsertHit(hit);
  }
  SetTimestamp(smallest_time);
  std::sort(mode3_hits.begin(),mode3_hits.end());
  return Size();
}

/*******************************************************************************/
/* Basic Print Function ********************************************************/
/*******************************************************************************/
void TMode3::Print(Option_t *opt) const {
  TString sopt(opt);
  sopt.ToLower();

  printf("TMode3;  %zu total hits:\n",Size());
  printf(" @ %lu \n",Timestamp());

  if(sopt.Contains("all")) {
    for(size_t i=0;i<Size();i++) {
      printf("\t");
      GetMode3Hit(i).Print();
    }
  }
}

/*******************************************************************************/
/* Clears Hit Vector ***********************************************************/
/*******************************************************************************/
void TMode3::Clear(Option_t *opt) {
  mode3_hits.clear();
}
