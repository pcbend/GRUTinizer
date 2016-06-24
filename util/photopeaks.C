// Gaussian + low-energy step
   Double_t fitf(Double_t *v, Double_t *par){
 
   static Float_t sqrt2pi = TMath::Sqrt(2*TMath::Pi()), sqrt2 = TMath::Sqrt(2.);

   Double_t arg;
   Double_t norm   = par[0];
   Double_t energy = par[1];
   Double_t sigma  = par[2];
   Double_t step   = par[3];
      
   arg = (v[0]-energy)/(sqrt2*sigma);

   Double_t result;
   result = 1/(sqrt2pi*sigma) * norm * exp(-arg*arg);
   result += step/pow(1+exp(sqrt2*arg),2);
 
   return result;
}


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

// Fit function
   TF1 *g = new TF1("peak", "gausn", energy-10., energy+10.);
   spectrum->Fit("peak", "RNQ");

   Double_t sigma = g->GetParameter(2);



//Printing the simple sum to the shell window
   Double_t sum 
      = spectrum->Integral( spectrum->FindBin(energy-3.75*sigma),
			    spectrum->FindBin(energy+3.75*sigma) );

   cout << "Simple sum = " << sum << " +/- " << sqrt(sum) << endl;



//Another fit
  TF1 *bg = new TF1("bg", "pol1", energy-10.*sigma, energy-4.*sigma);
  spectrum->Fit("bg", "RNQ");


// Fit the simulation to the measured spectrum
  const Int_t nPar = 4;
  Double_t par[1000];
  
// Set starting parameter values
  par[0]  = sum;
  par[1]  = energy;
  par[2]  = sigma;
  par[3]  = bg->Eval(energy-4.*sigma); 

  TF1 *fit = new TF1("fit", fitf, energy-8.*sigma, energy+5.*sigma, nPar);
  fit->SetParameters(par);
  spectrum->Fit("fit", "RNQ");
  return fit;

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

void photopeaks(){
  const Int_t npeaks = 29;
  Double_t energies [npeaks] = {244.7, 344.3, 411.1, 444.0, 778.9, 
                             867.4,  898.0,  964.0,  1085.8, 1089.7, 
                             1112.1, 1173.2, 1212.9, 1299.1, 
                             1332.5, 1408.0, 1836.0, 846.8, 
                             1037.9, 1238.3, 1360.2, 1771.4, 
                             2015.2, 2034.8, 2598.5, 3253.8, 
                             3273.0, 3451.2, 3548.3};
  Double_t denergies [npeaks] = {0};
  Double_t effic [npeaks];
  Double_t deffic [npeaks];

  for(Int_t i = 0; i < npeaks; i++){
     TString fileName;
     fileName.Form("e%.0f_histos.root", energies[i]);
     GPeak *fit = (GPeak *)fitpeak(fileName, energies[i]);
     effic[i] = fit->GetSum()/1.0e6*100;
     deffic[i] = fit->GetSumErr()/1.0e6*100;
     cout << fit->GetSum() << endl;
  }

  TGraphErrors *g1 = new TGraphErrors(npeaks, energies, effic, denergies, deffic);

  g1->SetTitle("GRETINA Standard 8 Quads");
  g1->GetXaxis()->SetTitle("Energy [keV]");
  g1->GetYaxis()->SetTitle("Efficiency [%]");
  g1->SetMarkerStyle(24);
  g1->SetMarkerColor(kBlue);
  g1->Draw("AP");
  gPad->SetCrosshair();

// Measured efficiencies (Dirk Weisshaar)
  //Double_t menergies[29] = { 244.7, 344.3, 411.1, 444.0, 778.9, 
                            // 867.4, 898.0, 964.0, 1085.8, 1089.7, 
                             //1112.1, 1173.2, 1212.9, 1299.1, 
                             //1332.5, 1408.0, 1836.0,  846.8, 
                             //1037.9, 1238.3, 1360.2, 1771.4, 
                             //2015.2, 2034.8, 2598.5, 3253.8, 
                             //3273.0, 3451.2, 3548.3};

  //Double_t dmenergies[29] = {0.};

  Double_t meffic[29] = { 12.30, 10.36, 9.24,  8.87,  6.59,  6.15,
                          6.07,  5.88,  5.59,  5.43,  5.55,  5.49,
                          5.10,  4.93,  5.07,  4.80,  4.09,  6.504,
                          5.780, 5.326, 5.128, 4.185, 3.891, 3.937,
                          3.314, 2.829, 2.826, 2.724, 2.789};

  Double_t dmeffic[29] =   { 0.19,  0.15,  0.15,  0.14,  0.10, 0.10,
                             0.05,  0.09,  0.09,  0.10,  0.08, 0.05,
                             0.09,  0.08,  0.05,  0.07,  0.03, 0.065,
                             0.063, 0.056, 0.060, 0.045, 0.050,    
                             0.045, 0.036, 0.094, 0.103, 0.125,  
                             0.168};
  
  TGraphErrors* mg1 = new TGraphErrors(npeaks, energies, meffic,
     denergies, dmeffic);

  mg1->SetTitle("GRETINA Standard 8 Quads");
  mg1->GetXaxis()->SetTitle("Energy [keV]");
  mg1->GetYaxis()->SetTitle("Efficiency [%]");
  mg1->SetMarkerStyle(20);
  mg1->SetMarkerColor(kBlack);
  
  mg1->Draw("P");
  
}
