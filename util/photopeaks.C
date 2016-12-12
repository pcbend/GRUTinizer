
// Gaussian + low-energy step + linear background
Double_t gstep(Double_t *v, Double_t *par){
 
   static Float_t sqrt2pi = TMath::Sqrt(2*TMath::Pi()), sqrt2 = TMath::Sqrt(2.);

   Double_t bglo1  = par[0];
   Double_t bghi0  = par[1];
   Double_t bghi1  = par[2];
   Double_t energy = par[3];
   Double_t sigma  = par[4];
   Double_t step   = par[5];
   Double_t norm   = par[6];
   
   Double_t arg = (v[0]-energy)/(sqrt2*sigma);
   Double_t result = bghi0 + bghi1*(v[0]-energy);
   result += 1/(sqrt2pi*sigma) * norm * exp(-arg*arg);
   result += (step + bglo1*(v[0]-energy))/pow(1+exp(sqrt2*arg),2);
   //   result += step/pow(1+exp(sqrt2*arg),2);
   //   result += step*TMath::Erfc( arg );
 
   return result;
   
}

// low-energy step + linear background
Double_t bgstep(Double_t *v, Double_t *par){
 
   static Float_t sqrt2pi = TMath::Sqrt(2*TMath::Pi()), sqrt2 = TMath::Sqrt(2.);

   Double_t bglo1  = par[0];
   Double_t bghi0  = par[1];
   Double_t bghi1  = par[2];
   Double_t energy = par[3];
   Double_t sigma  = par[4];
   Double_t step   = par[5];
   
   Double_t arg = (v[0]-energy)/(sqrt2*sigma);
   Double_t result = bghi0 + bghi1*(v[0]-energy);
   result += (step + bglo1*(v[0]-energy))/pow(1+exp(sqrt2*arg),2);
   //   result += step/pow(1+exp(sqrt2*arg),2);
   //   result += step*TMath::Erfc( arg );
   
   return result;
   
}

// Linear background (centered on the peak)
Double_t bghi(Double_t *v, Double_t *par){
 
   Double_t bghi0  = par[0];
   Double_t bghi1  = par[1];
   Double_t energy = par[2];
   
   Double_t result = bghi0 + bghi1*(v[0]-energy);
   
   return result;
   
}

Double_t bglo(Double_t *v, Double_t *par){
 
   Double_t bghi0  = par[0];
   Double_t bghi1  = par[1];
   Double_t bglo0  = par[2];
   Double_t bglo1  = par[3];
   Double_t energy = par[4];

   Double_t result = bghi0 + bghi1*(v[0]-energy)
                   + bglo0 + bglo1*(v[0]-energy);
   
   return result;
   
}

TF1* fitPeak(TString fileName, TString spectrumName,
	     Double_t llim, Double_t lpeak, Double_t upeak, Double_t ulim,
	     Double_t energy){

  //============================================================================
  // Load the spectrum.
  
  TFile *f = new TFile(fileName);
  GH1D *spectrum;
  f->GetObject(spectrumName, spectrum);
  
  //============================================================================
  Double_t sum = spectrum->Integral(spectrum->FindBin(llim),
				    spectrum->FindBin(ulim));
    
  // Fit the background above the photopeak.
  cout << "Fitting the background above the peak ("
       << upeak << ", " << ulim << ")." << endl;

  Double_t bgpar[3];
  bgpar[0] = spectrum->GetBinContent(spectrum->FindBin(ulim));
  bgpar[1] = 0.;
  bgpar[2] = energy;
  
  TF1 *bghi  = new TF1("bghi", bghi, upeak, ulim, 3);
  bghi->SetParameters(bgpar);
  
  if(spectrum->Integral(upeak, ulim) > ulim - upeak){

    bghi->FixParameter(2, bgpar[2]);
    spectrum->Fit("bghi", "R");

  } else
    cout << "Found zero background above the peak." << endl;

  // Fit the background below the photopeak.
  cout << "Fitting the background below the peak ("
       << llim << ", " << lpeak << ")." << endl;

  Double_t bglopar[5];
  bglopar[0] = bghi->GetParameter(0);
  bglopar[1] = bghi->GetParameter(1);
  bglopar[2] = spectrum->GetBinContent(spectrum->FindBin(llim))
             - spectrum->GetBinContent(spectrum->FindBin(ulim));
  bglopar[3] = 0.;
  bglopar[4] = energy;

  TF1 *bglo  = new TF1("bglo", bglo, llim, lpeak, 5);
  bglo->SetParameters(bglopar);
  
  bglo->FixParameter(0, bglopar[0]);
  bglo->FixParameter(1, bglopar[1]);
  bglo->FixParameter(4, bglopar[4]);

  spectrum->Fit("bglo", "R");

  cout << "Fitting a Gaussian on the step." << endl;

  // Fit a Gaussian on the step
  Double_t par[7];
  par[0] = bglo->GetParameter(3);
  par[1] = bghi->GetParameter(0);
  par[2] = bghi->GetParameter(1);
  par[3] = energy;
  par[4] = energy/1000.; // just a rough estimate of the resolution
  par[5] = bghi->Eval(energy) - bglo->Eval(energy); 
  par[6] = sum;
  
  TF1 *gstep = new TF1("gstep", gstep, llim, ulim, 7);
  gstep->SetParameters(par);

  gstep->FixParameter(0, par[0]);
  gstep->FixParameter(1, par[1]);
  gstep->FixParameter(2, par[2]);
  gstep->SetParLimits(4,.2,10.);
  gstep->SetParLimits(6,0.,sum);
  
  gstep->SetLineColor(kGreen);
  spectrum->GetXaxis()->SetRangeUser(2*llim - energy, 2*ulim - energy);

  spectrum->Fit("gstep", "R");

  cout << "Simple sum:                "
       << sum << " +/- " << sqrt(sum)
       << endl;

  cout << "Norm:                      "
       << gstep->GetParameter(6)
       << endl;

  return gstep;

}

// TF1 *fitpeak(TString fileName, Double_t energy){
// //Printing the graph
//    TFile *f = new TFile(fileName);
//    TH1 *spectrum;

//    f->GetObject("energy/energy_gaus", spectrum);

//    GPeak *g = new GPeak (energy,energy-15.,energy+8.);
//    g->Fit(spectrum,"Q");

//    return g;

// }

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
     // GPeak *fit = (GPeak *)fitpeak(fileName, energies[i]);
     // effic[i] = fit->GetSum()/1.0e6*100;
     // deffic[i] = fit->GetSumErr()/1.0e6*100;


     // Set fit and peak boundaries based on simple energy/1000.
     // Gaussian smearing of simulated spectra.
     Double_t dlim  = energies[i]*0.0043711887 + 13.9;
     Double_t llim  = energies[i]-dlim;
     Double_t ulim  = energies[i]+dlim;

     Double_t dpeak = energies[i]*0.0028219811 + 4.2;
     Double_t lpeak = energies[i]-dpeak;
     Double_t upeak = energies[i]+dpeak;

     TF1 * fit = fitPeak(fileName, "energy/energy_gaus",
			 llim, lpeak, upeak, ulim,
			 energies[i]);
     effic[i]  = fit->GetParameter(6)/1.0e6*100.;
     deffic[i] = fit->GetParError(6)/1.0e6*100.;
  }

  TGraphErrors *g1 = new TGraphErrors(npeaks, energies, effic, denergies, deffic);

  g1->SetTitle("GRETINA Photopeak Efficiencies");
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
