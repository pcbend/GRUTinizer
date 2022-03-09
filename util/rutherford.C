#include "kinematics.C"

void rutherford(double zoff = 0.0) {

  TF1* fLAB = new TF1("fCM","sin(x)*RuthLAB(x)",0,TMath::Pi());

  double csSum = 0.0;
  TGraph* g = new TGraph(24);
  g->SetMarkerStyle(20);
  g->SetMarkerSize(2);
  g->SetMarkerColor(kBlue);
  g->SetLineColor(kBlue);
  g->SetLineWidth(2);

  TJanusDDAS j;
  for(int i=0;i<24;i++) {

    TVector3 pos1 = j.GetPosition(1,i+1,1);
    TVector3 pos2 = j.GetPosition(1,i+1,1);

    pos1.SetPerp(pos1.Perp() - 0.05);
    pos2.SetPerp(pos2.Perp() + 0.05);

    pos1.SetZ(pos1.Z() + zoff);
    pos2.SetZ(pos2.Z() + zoff);

    double cs = fLAB->Integral(pos1.Theta(),pos2.Theta())*2.0*TMath::Pi();
    g->SetPoint(i,i+1.5,cs);

    if(i >= 11) {
      csSum += cs;
    }

  }

  TFile* f = new TFile("~/analysis/e20011/Sn112_Platinum/bigHists/master.root");
  GH1* h = (GH1*)f->Get("Primary_Janus_sn112/All_Ring_det1");
  h->SetLineColor(kBlack);
  h->SetLineWidth(2);
  
  double tot_counts = h->Integral(13,25);
  for(int i=0;i<24;i++) {
    g->SetPoint(i,i+1.5,g->GetY()[i]*tot_counts/csSum);
  }
  
  h->Draw("hist");
  g->Draw("same PL");
  
  return;
}
