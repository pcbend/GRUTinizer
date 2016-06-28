
TF1 *fitpeak(TString fileName, Double_t energy){
//Printing the graph
   TFile *f = new TFile(fileName);
   TH1 *spectrum;
   TH2 *overview;

   f->GetObject("energy/overview_gaus", overview);
   spectrum = overview->ProjectionX();

   //spectrum->Draw();

   GPeak *g = new GPeak (energy,energy-10,energy+5);
   g->Fit(spectrum,"Q");

   return g;

}

void photopeaks(){
  // const Int_t npeaks = 29;
  // Double_t energies [npeaks] = {244.7, 344.3, 411.1, 444.0, 778.9, 
  //                            867.4,  898.0,  964.0,  1085.8, 1089.7, 
  //                            1112.1, 1173.2, 1212.9, 1299.1, 
  //                            1332.5, 1408.0, 1836.0, 846.8, 
  //                            1037.9, 1238.3, 1360.2, 1771.4, 
  //                            2015.2, 2034.8, 2598.5, 3253.8, 
  //                            3273.0, 3451.2, 3548.3};
  const Int_t npeaks = 25;
  Double_t energies [npeaks] = {244.7,  344.3,  411.1, 444.0,   778.9, 
                                867.4,  964.0, 1085.8, 1089.7, 1112.1,
			       1212.9, 1299.1, 1408.0,
				846.8, 1037.9, 1238.3, 1360.2, 1771.4, 
                               2015.2, 2034.8, 2598.5, 3253.8, 3273.0,
			       3451.2, 3548.3};

  Double_t denergies [npeaks] = {0};
  Double_t effic [npeaks];
  Double_t deffic [npeaks];

  for(Int_t i = 0; i < npeaks; i++){
     TString fileName;
     fileName.Form("e%.0f_histos.root", energies[i]);
     GPeak *fit = (GPeak *)fitpeak(fileName, energies[i]);
     effic[i] = fit->GetSum()/1.0e6*100;
     deffic[i] = fit->GetSumErr()/1.0e6*100;
  }

  TGraphErrors *g1 = new TGraphErrors(npeaks, energies, effic, denergies, deffic);

  g1->SetTitle("GRETINA Standard 8 Quads");
  g1->GetXaxis()->SetTitle("Energy [keV]");
  g1->GetYaxis()->SetTitle("Efficiency [%]");
  g1->SetMarkerStyle(24);
  g1->SetMarkerColor(kBlue);
  g1->Draw("AP");
  gPad->SetCrosshair();
  
// Fit to measured efficiencies (Dirk Weisshaar)
  TGraph *fg1 = new TGraph();
  Double_t dE = 3700./100.;
  Double_t E = 0.;

  for(Int_t i=0; i<100; i++){
    fg1->SetPoint(i,E, 706.1*exp(-0.678095*log(E + 160)) );
    E += dE;
  }

  fg1->SetLineColor(kBlue);
  fg1->Draw("C");

}
