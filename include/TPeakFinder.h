#ifndef TPEAKFINDER_H
#define TPEAKFINDER_H

#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#ifndef __CINT__
#include <sstream>
#endif

#include "TObject.h"
#include "TList.h"
#include "TFile.h"
#include "TClass.h"
#include "TKey.h"
#include "TSpectrum.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TRandom.h"

#include "TNucleus.h"
#include "ProgramPath.h"

class TPeakFinder : public TObject{
 public:
  TPeakFinder(TNucleus *nucl);
  ~TPeakFinder();
  void Clear(Option_t *opt="");
  void Print(Option_t *opt="") const;
  
  TH1 *GetBackground(TH1* hist);
  TH1 *GetBgSub(TH1* hist);
  TH1 *GetScaled(TH1* hist,Double_t cal);
  TH1 *GetBackground() const    { if(hBkgd) return hBkgd; else return 0;}
  TH1 *GetBgSub() const         { if(hBkgdSub) return hBkgdSub; else return 0;}
  TH1 *GetScaled() const        { if(hScaled) return hScaled; else return 0;}
  Double_t GetPeaksForFit()     { return fPeakNum; }
  TNucleus *GetNucleus() const  { return tnNucleus; }
  

  //TSpectrum *LocatePeaks(TH1* hist,Double_t sigma=2.0,Double_t thresh=0.2);
  //void OrderPeaks(TSpectrum* Spec,Double_t &XVal,Double_t &YVal,Bool_t LowToHigh=true);


  

 private:
  Bool_t PeaksForFit(); // Only call this in the constructor.
  std::vector<Double_t> fPeaksToFit;
  Double_t fPeakNum;
  TNucleus *tnNucleus;
  TH1 *hBkgd;
  TH1 *hBkgdSub;
  TH1 *hScaled;
  ClassDef(TPeakFinder,1);
};



#endif
