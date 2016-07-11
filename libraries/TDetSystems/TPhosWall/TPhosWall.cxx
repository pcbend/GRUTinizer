#include "TPhosWall.h"

#include "TFile.h"
#include "TKey.h"

#include "TH2.h"
#include "TCutG.h"
#include "GCanvas.h"
#include "TMath.h"
#include "TStyle.h"
#include "TPaveStats.h"

#include "TNucleus.h"
#include "GRootCommands.h"

ClassImp(TPhosWall)

TList TPhosWall::gates;

std::vector<TVector3> TPhosWall::fWallPositions(257);
bool TPhosWall::fPositionsSet = false;

TPhosWall::TPhosWall() {
  if(!fPositionsSet)
    SetWallPositions();
}

TPhosWall::~TPhosWall() { }


void TPhosWall::Copy(TObject &rhs) const {
  TDetector::Copy(rhs);

  TPhosWall det = (TPhosWall&)rhs;

  det.phoswall_hits = phoswall_hits;
  det.fTimestamp    = fTimestamp;
  det.fLargestHit   = fLargestHit;
  det.fNumberOfHits = fNumberOfHits;

}

void TPhosWall::Clear(Option_t *opt) {
  TDetector::Clear(opt);

  phoswall_hits.clear();

  fTimestamp    = -1;
  fLargestHit   = -1;
  fNumberOfHits = -1;

}

void TPhosWall::SortHits() {
  std::sort(phoswall_hits.begin(),phoswall_hits.end());
}



void TPhosWall::Print(Option_t *opt) const {
  TString option(opt);
  std::cout << "TPhosWall:  "  << fTimestamp << "    " <<  fLargestHit << std::endl;
  if(option.Contains("all")) {
    printf("\tcurrently holding %i gates:\n",gates.GetSize());
    for(int i=0;i<gates.GetSize();i++) {
      printf("\t\t%s\n",GetGateName(i));
    }
  }
  std::cout << "             A       B       C     Time"  << std::endl;
  for(int i = 0; i<int(Size()); i++) {
    if(i==fLargestHit)
      std::cout << DMAGENTA;
    GetPhosWallHit(i).Print(opt);
    if(i==fLargestHit)
      std::cout << RESET_COLOR;
  }
}

int TPhosWall::BuildHits(std::vector<TRawEvent>& raw_data) {

  for(size_t i=0;i<raw_data.size();i++) {
    TGEBEvent &geb = (TGEBEvent&)raw_data.at(i);
    fTimestamp = geb.GetTimestamp();
    fLargestHit = 0;
    for(int j=0;j<(geb.GetBodySize()-8); j+=sizeof(TRawEvent::PWHit)) {
      TPhosWallHit hit((TRawEvent::PWHit*)(geb.GetPayload() + j));
      hit.SetTimestamp(geb.GetTimestamp());
      hit.SetAddress(0xbaff0000 + hit.Pixel());
      InsertHit(hit);
      if(hit.Pixel()>255)
        geb.Print("all");
    }
  }

  return Size();
}

void TPhosWall::InsertHit(const TDetectorHit &hit) {
  phoswall_hits.push_back((TPhosWallHit&)hit);
}





void TPhosWall::SetWallPositions() {
  printf("Setting PhosWall positions.\n");
  int detoffset;
  double phi;
  for(int i=0;i<4;i++) {
    if(i==0)      {detoffset = 0;    phi = TMath::DegToRad()*0   + TMath::PiOver2() ;}
    else if(i==1) {detoffset = 64;   phi = TMath::DegToRad()*270 + TMath::PiOver2() ;}
    else if(i==2) {detoffset = 64*2; phi = TMath::DegToRad()*180 + TMath::PiOver2() ;}
    else if(i==3) {detoffset = 64*3; phi = TMath::DegToRad()*90  + TMath::PiOver2() ;}

    fWallPositions[0 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*37.141,TMath::DegToRad()*20.966 +  phi);
    fWallPositions[1 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*32.895,TMath::DegToRad()*24.772 +  phi);
    fWallPositions[2 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*28.445,TMath::DegToRad()*30.019 +  phi);
    fWallPositions[3 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*23.908,TMath::DegToRad()*37.687 +  phi);
    fWallPositions[4 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*19.653,TMath::DegToRad()*49.364 +  phi);
    fWallPositions[5 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*16.391,TMath::DegToRad()*67.124 +  phi);
    fWallPositions[6 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*15.165,TMath::DegToRad()*89.178 + phi);
    fWallPositions[7 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*16.580,TMath::DegToRad()*65.538 + phi);
    fWallPositions[8 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*40.971,TMath::DegToRad()*26.144 +  phi);
    fWallPositions[9 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*36.941,TMath::DegToRad()*30.589 +  phi);
    fWallPositions[10 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*32.748,TMath::DegToRad()*36.507 +  phi);
    fWallPositions[11 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*28.546,TMath::DegToRad()*44.701 +  phi);
    fWallPositions[12 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*24.730,TMath::DegToRad()*56.18  +  phi);
    fWallPositions[13 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*21.943,TMath::DegToRad()*71.77 +   phi);
    fWallPositions[14 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*20.940,TMath::DegToRad()*89.358 + phi);
    fWallPositions[15 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*22.100,TMath::DegToRad()*70.448 + phi);

    fWallPositions[16 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*45.272,TMath::DegToRad()*30.836  + phi);
    fWallPositions[17 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*41.556,TMath::DegToRad()*35.713  + phi);
    fWallPositions[18 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*37.721,TMath::DegToRad()*41.992  + phi);
    fWallPositions[19 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*33.932,TMath::DegToRad()*50.277  + phi);
    fWallPositions[20 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*30.574,TMath::DegToRad()*61.151  + phi);
    fWallPositions[21 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*28.196,TMath::DegToRad()*74.846  + phi);
    fWallPositions[22 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*27.361,TMath::DegToRad()*89.472 + phi);
    fWallPositions[23 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*28.328,TMath::DegToRad()*73.722 + phi);

    fWallPositions[24 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*50.035,TMath::DegToRad()*35.110  + phi);
    fWallPositions[25 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*46.724,TMath::DegToRad()*40.255  + phi);
    fWallPositions[26 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*43.330,TMath::DegToRad()*46.672  + phi);
    fWallPositions[27 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*40.027,TMath::DegToRad()*54.797  + phi);
    fWallPositions[28 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*37.149,TMath::DegToRad()*64.933  + phi);
    fWallPositions[29 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*35.153,TMath::DegToRad()*77.050  + phi);
    fWallPositions[30 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*34.462,TMath::DegToRad()*89.552 + phi);
    fWallPositions[31 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*35.263,TMath::DegToRad()*76.075 + phi);

    fWallPositions[32 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*55.164,TMath::DegToRad()*38.979  + phi);
    fWallPositions[33 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*52.326,TMath::DegToRad()*44.260  + phi);
    fWallPositions[34 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*49.446,TMath::DegToRad()*50.663  + phi);
    fWallPositions[35 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*46.672,TMath::DegToRad()*58.492  + phi);
    fWallPositions[36 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*44.293,TMath::DegToRad()*67.884  + phi);
    fWallPositions[37 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*42.667,TMath::DegToRad()*78.701  + phi);
    fWallPositions[38 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*42.109,TMath::DegToRad()*89.611 + phi);
    fWallPositions[39 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*42.756,TMath::DegToRad()*77.843 + phi);

    fWallPositions[40 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*60.528,TMath::DegToRad()*42.468  + phi);
    fWallPositions[41 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*58.205,TMath::DegToRad()*47.786  + phi);
    fWallPositions[42 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*55.871,TMath::DegToRad()*54.074  + phi);
    fWallPositions[43 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*53.651,TMath::DegToRad()*61.546  + phi);
    fWallPositions[44 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*51.772,TMath::DegToRad()*70.238  + phi);
    fWallPositions[45 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*50.503,TMath::DegToRad()*79.983  + phi);
    fWallPositions[46 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*50.071,TMath::DegToRad()*89.656 + phi);
    fWallPositions[47 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*50.572,TMath::DegToRad()*79.217 + phi);

    fWallPositions[48 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*65.983,TMath::DegToRad()*45.606  + phi);
    fWallPositions[49 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*64.180,TMath::DegToRad()*50.890  + phi);
    fWallPositions[50 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*62.390,TMath::DegToRad()*57.005  + phi);
    fWallPositions[51 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*60.711,TMath::DegToRad()*64.098  + phi);
    fWallPositions[52 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*59.308,TMath::DegToRad()*72.154  + phi);
    fWallPositions[53 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*58.371,TMath::DegToRad()*81.006  + phi);
    fWallPositions[54 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*58.054,TMath::DegToRad()*89.692 + phi);
    fWallPositions[55 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*58.422,TMath::DegToRad()*80.315 + phi);

    fWallPositions[56 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*71.343,TMath::DegToRad()*48.456  + phi);
    fWallPositions[57 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*70.127,TMath::DegToRad()*53.656  + phi);
    fWallPositions[58 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*68.848,TMath::DegToRad()*59.560  + phi);
    fWallPositions[59 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*67.665,TMath::DegToRad()*66.274  + phi);
    fWallPositions[60 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*66.691,TMath::DegToRad()*73.755  + phi);
    fWallPositions[61 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*66.047,TMath::DegToRad()*81.484  + phi);
    fWallPositions[62 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*65.831,TMath::DegToRad()*89.721 + phi);
    fWallPositions[63 + detoffset].SetMagThetaPhi(55.0,TMath::DegToRad()*66.082,TMath::DegToRad()*81.219 + phi);
  }
  fWallPositions[256].SetXYZ(0,0,1);

  fPositionsSet=true;
}

TVector3 TPhosWall::GetWallPosition(int pixelnumber, double delta){
  try{
    TVector3 vec = fWallPositions.at(pixelnumber);
    //printf("known PhosWall pixel number: 0x%04x\n\n", pixelnumber);
    return vec;
    //return fWallPositions.at(pixelnumber);
  } catch (std::out_of_range& e){
    //std::cout << "Unknown PhosWall pixel number: " << pixelnumber << "\n\n" << std::endl;
    printf("Unknown PhosWall pixel number: 0x%04x\n\n", pixelnumber);
    return TVector3(sqrt(-1), sqrt(-1), sqrt(-1));
  }
}

// TVector3 TPhosWall::GetWallPosition(int pixelnumber, double delta){

//    //printf("Calculating PhosWall positions.\n");

//    int det   = pixelnumber/64;
//    int pixel = pixelnumber - (64*det);
//    TVector3 position;

//    double L   = 49.0;
//    double Ro  = 55.0;
//    double Psi = 50.6*TMath::DegToRad();

//    int x,y;  //1 to 8!!
//    switch(det) {
//       case 0:
//          x = (pixel%8);
//          y = (pixel/8);
//          break;
//       case 1:
//          x = (pixel/8) + 1;
//          y = abs((pixel%8)-7);
//          break;
//       case 2:
//          x = abs((pixel%8)-7);
//          y = abs((pixel/8)-7);
//          break;
//       case 3:
//          x = abs((pixel/8)-7);
//          y = (pixel&8) + 1;
//          break;
//    }
//    x++; y++;
//    double localx = ((double(x)-4.5)*6.08);
//    double localy = ((double(y)-4.5)*6.08);
//    //------------------------------------------------//
//    //------------------------------------------------//
//    //------------------------------------------------//
//    //------------------------------------------------//
//       // step sizes are awkward.  from 1-8 in either x or y:
//       //
//       // 1:    3.13
//       // 2:    9.30
//       // 3:   15.38
//       // 4:   21.46
//       // 5:   27.54
//       // 6:   32.62
//       // 7:   39.70
//       // 8:   45.87
//    //------------------------------------------------//
//    //------------------------------------------------//
//    //------------------------------------------------//
//    //------------------------------------------------//

//    double globalx = (L/2.0) - localx + delta;
//    double globaly = Ro*TMath::Sin(Psi) - ((L/2.0)-localy)*TMath::Cos(Psi) - delta;
//    double globalz = Ro*TMath::Cos(Psi) + ((L/2.0)-localy)*TMath::Sin(Psi);

//    position.SetXYZ(globalx,globaly,globalz);
//    position.RotateZ(((double)(det))*TMath::PiOver2());

//    return position;
// }
//
//


void TPhosWall::Draw(Option_t *opt) {
  TH2I hitpat[4];
  //std::string exef;
  //std::string exex = ".x ";
  //if(strlen(opt)!=0) {
  //  exef.append(opt);
  //  exex.append(opt);
  //  exex.append(".C");
  //}
  for(int x=0;x<4;x++) {
    hitpat[x] = hitpat[x] = TH2I(Form("hit%i",x),Form("Pixels %03i - %03i",x*64,x*64+63),8,0,8,8,0,8);
    hitpat[x].GetXaxis()->SetNdivisions(8);
    hitpat[x].GetYaxis()->SetNdivisions(8);
    hitpat[x].SetStats(false);
  }
  for(unsigned int x=0;x<Size();x++) {
    int hist = Pixel(x)/64;
    int row  = (Pixel(x)-(hist*64))%8;
    int col  = (Pixel(x)-(hist*64))/8;
    hitpat[hist].Fill(row,col,B(x));
    printf("\t\tPixel[%03i | %02i]  r[%02i] c[%02i]    %i\n",Pixel(x),Pixel(x)-hist*64,row,col,B(x));
  }
  if(!gPad || !gPad->IsEditable())  {
    gROOT->MakeDefCanvas();
  } else {
    gPad->GetCanvas()->Clear();
  }
  TVirtualPad *mother = gPad;
  mother->Divide(2,2);
  mother->cd(1); hitpat[0].DrawCopy("colz");
  gPad->SetGrid();
  ///if(exef.size()) gPad->AddExec(exef.c_str(),exex.c_str());
  mother->cd(2); hitpat[1].DrawCopy("colz");
  gPad->SetGrid();
  //if(exef.size()) gPad->AddExec(exef.c_str(),exex.c_str());
  mother->cd(4); hitpat[2].DrawCopy("colz");
  gPad->SetGrid();
  //if(exef.size()) gPad->AddExec(exef.c_str(),exex.c_str());
  mother->cd(3); hitpat[3].DrawCopy("colz");
  gPad->SetGrid();
  //if(exef.size()) gPad->AddExec(exef.c_str(),exex.c_str());
  gPad->Modified();
  gPad->Update();
  return;
}

void TPhosWall::DrawPID(Option_t *gate,Option_t *opt,Long_t nentries,TChain *chain) {
  if(!chain)
    chain = gChain;
  if(!chain || !chain->GetBranch("TPhosWall"))
    return;
  //printf("chain = %s\t%lu\t%lu\n",chain->GetName(),chain->GetEntries(),nentries);
  if(!gPad || !gPad->IsEditable())  {
    gROOT->MakeDefCanvas();
  } else {
    gPad->GetCanvas()->Clear();
  }
  std::string name = Form("%s_PID",Class()->GetName()); //_%s",opt);
  //const char *name = "phoswall_pid";
  GH2I *h = new GH2I(name.c_str(),name.c_str(),2048,0,8192,2048,0,8192);
  chain->Project(name.c_str(),"phoswall_hits.C():phoswall_hits.B()",gate,"",nentries);
  //chain->Draw("phoswall_hits.C():phoswall_hits.B()>>h","","colz",10000); //nentries);
  h->GetXaxis()->SetTitle("Largest B");
  h->GetYaxis()->SetTitle("Largest C");
  h->Draw("colz");
  //TCanvas *C1 = new TCanvas("C1");
  //chain->Draw("phoswall_hits.C():phoswall_hits.B()","","colz",10000);
}

TVector3 TPhosWall::GetKinVector(Double_t E_ejec,Double_t E_beam,const char *beam,const char *recoil, const char *ejec) {
  TNucleus nbeam(beam);
  TNucleus nejec(ejec);
  TNucleus nrecoil(recoil);
  return GetKinVector(E_beam,E_ejec,nbeam,nrecoil,nejec);
}

TVector3 TPhosWall::GetKinVector(Double_t E_ejec,Double_t E_beam,TNucleus &beam,TNucleus &recoil, TNucleus &ejec) {

  double v_beam = sqrt(2*E_beam/beam.GetMass());
  double p_beam = beam.GetMass()*v_beam;

  double v_ejec = sqrt(2*E_ejec/ejec.GetMass());
  double p_ejec = ejec.GetMass()*v_ejec;

  double p_recoil     = sqrt(pow(p_beam-p_ejec*GetHitPosition().CosTheta(),2) + pow(p_ejec*TMath::Sin(GetHitPosition().Theta()),2));
  double theta_recoil = TMath::ASin((p_ejec/p_recoil)*TMath::Sin(GetHitPosition().Theta()));
  double phi_recoil   = GetHitPosition().Phi();
  if(phi_recoil<TMath::Pi())
    phi_recoil += TMath::Pi();
  else if(phi_recoil > TMath::Pi())
    phi_recoil -= TMath::Pi();
  TVector3 v;
  v.SetMagThetaPhi(1,theta_recoil,phi_recoil);
  return v;
}

int TPhosWall::IsInside(Option_t *opt) const {
  if(!gates.GetSize() || !Size())
    return 0;
  TIter iter(&gates);
  int counter =1;
  while(TCutG* cut = (TCutG*)iter.Next()) {
    if(cut->IsInside(B(fLargestHit),C(fLargestHit)))
      return counter;
    counter++;
  }
  return 0;
}

int TPhosWall::LoadGates(const char *filename) {
  TDirectory *current = gDirectory;
  TFile *f = new TFile(filename);
  int found=0;
  if(f->IsOpen()) {
    TIter iter(f->GetListOfKeys());
    while(TKey *key = (TKey*)iter.Next()) {
      if(!strncmp("TCutG",key->GetClassName(),5)) {
        gates.Add((TCutG*)key->ReadObj());
        found++;
      }
    }
  }
  f->Close();
  f->Delete();
  current->cd();
  return found;
}

int TPhosWall::SaveGates(const char *filename) {
  if(!gates.GetSize()) {
    printf("%s, currently no gates to save.\n",__PRETTY_FUNCTION__);
    return 0;
  }
  TDirectory *current = gDirectory;
  std::string fname = filename;
  if(!fname.length())
    fname.assign("phoswall_gates.root");
  TFile *f = new TFile(fname.c_str(),"update");

  f->mkdir("TPhosWall");
  f->cd("TPhosWall");
  gates.Sort();
  gates.Write();
  f->Close();
  f->Delete();
  current->cd();
  return gates.GetSize();
}

/*
void TPhosWall::DrawXY(Option_t *opt) {
  TH2I hitpattern("hitpattern","PWFragmet XY Hit Pattern",
                   116,-58,58,116,-58,58);
  for(int i=0;i<fNumberOfHits;i++) {
     TVector3 vec = GetWallPosition(Pixel(i));
     hitpattern.Fill(vec.X(),
                     vec.Y(),
                     A(i));
  }
  if(gPad || !gPad->IsEditable())
    gROOT->MakeDefCanvas();
  hitpattern.DrawCopy("colz");
  gPad->SetGrid();
  gPad->Modified();
  gPad->Update();
  return;
}
*/
