#include <GH1.h>
#include <TTree.h>
#include <TS800.h>
#include <TGretina.h>
#include <TFile.h>


void filterTree(TTree* gChain,const char* opf){
  TS800 *s800;
  TGretina *gretina;
  gChain->SetBranchAddress("TS800",&s800);
  gChain->SetBranchAddress("TGretina",&gretina);

  Double_t tof;
  Double_t dE;
  Int_t    ng;
  Double_t gammae[20];
  Double_t tdiff[20];
  Double_t theta[20];

  TFile *opfile = new TFile(opf,"RECREATE");
  TTree *opt = new TTree("opt","opt");
  opt->Branch("tof",&tof,"tof/D");
  opt->Branch("dE",&dE,"dE/D");
  opt->Branch("ng",&ng,"ng/I");
  opt->Branch("gammae",&gammae,"gammae[ng]/D");
  opt->Branch("tdiff",&tdiff,"tdiff[ng]/D");
  opt->Branch("theta",&theta,"theta[ng]/D");

  for(long long i = 0; i<gChain->GetEntries(); i++){
    gChain->GetEntry(i);
    if(s800->Size() == 0 || gretina->Size() == 0) return;

    ng = gretina->Size();
    tof = s800->GetCorrTOF_OBJ_MESY();
    dE = s800->GetIonChamber().GetdE(&s800->GetCrdc(0));

    for(size_t x = 0; x<gretina->Size();x++){
      tdiff[x] = s800->Timestamp()-gretina->GetGretinaHit(x).Timestamp();
      gammae[x] = gretina->GetGretinaHit(x).GetEnergy();
      theta [x] = gretina->GetGretinaHit(x).GetTheta();
    }
    opt->Fill();
  }
  opt->Write();
  opfile->Close();
}
