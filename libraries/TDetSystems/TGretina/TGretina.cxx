#include "TGretina.h"

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
  fCRMATSet = true;
  //printf("Read %i rotation matrix coefficients.\n", nn);
  /* Done! */
}

void TGretina::SetSegmentCRMAT() {
  if(fCRMATSet)
    return;
  FILE *infile;
  int NUMSEG = 36;
  std::string temp = getenv("GRUTSYS");
  temp.append("/libraries/TDetSystems/TGretina/crmat.dat");
  infile = fopen(temp.c_str(),"r");
  if(!infile) {
    return;
  }
  // notice: In file type-A is first, then 
  //         type-B but as type-B are even
  //         det_ids in the data stream we
  //         define type=0 for type-B not
  //         type-A.
  for(int type=0;type<2;type++) {
    for(int seg=0;seg<NUMSEG;seg++) {
      int rtval,segread;
      rtval=fscanf(infile, "%d %lf %lf %lf",
                   &segread,
                   m_segpos[(type+1)%2][seg][0],
                   m_segpos[(type+1)%2][seg][1],
                   m_segpos[(type+1)%2][seg][2]);
      if((seg+1)!=segread) {
        fprintf(stderr,"%s: seg[%i] read but seg[%i] expected.\n",__PRETTY_FUNCTION__,segread,seg+1);
      }
      if(rtval==EOF) {
        fprintf(stderr,"%s: unexpected EOF.\n",__PRETTY_FUNCTION__);
        break;
      }
    }
  }
  fclose(infile);
}


TVector3 TGretina::GetSegmentPosition(int cry_id,int segment) {
  SetCRMAT();
  float x = m_segpos[cry_id%2][segment][0];
  float y = m_segpos[cry_id%2][segment][1];
  float z = m_segpos[cry_id%2][segment][2];
  return CrystalToGlobal(cry_id,x,y,z);
}

void TGretina::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TGretina& gretina = (TGretina&)obj;
  gretina.gretina_hits = gretina_hits; // gretina_hits->Copy(*gretina.gretina_hits);
  //addback_hits->Copy(*gretina.addback_hits);
  gretina.raw_data.clear();
}

void TGretina::InsertHit(const TDetectorHit& hit){
  //TGretinaHit* new_hit = (TGretinaHit*)gretina_hits->ConstructedAt(Size());
  //hit.Copy(*new_hit);
  gretina_hits.emplace_back((TGretinaHit&)hit);
  fSize++;
}

int TGretina::BuildHits(){
  //printf("%s\n",__PRETTY_FUNCTION__);
  for(auto& event : raw_data){
    TGEBEvent* geb = (TGEBEvent*)&event;
    SetTimestamp(geb->GetTimestamp());
    const TRawEvent::GEBBankType1* raw = (const TRawEvent::GEBBankType1*)geb->GetPayloadBuffer().GetData();
    TGretinaHit hit;
    hit.BuildFrom(*raw);
    InsertHit(hit);
  }
  //gretina_hits->At(0)->Print();
  raw_data.clear();

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
        GetAddbackHit(y).AddToSelf(GetGretinaHit(x), max_energies[y]);
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

void TGretina::Print(Option_t *opt) const { }

void TGretina::Clear(Option_t *opt) {
  TDetector::Clear(opt);
  gretina_hits.clear(); //("TGretinaHit");
  //addback_hits->Clear(opt);//("TGretinaHit");
  raw_data.clear();
}
