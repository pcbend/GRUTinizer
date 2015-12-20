#include <TPeakFinder.h>

ClassImp(TPeakFinder)

TPeakFinder::TPeakFinder(TNucleus *nucl){
  Clear();
  tnNucleus = nucl;
  std::cout << "> Transitions for " << tnNucleus->GetName() 
	    << " loaded." << std::endl;  

  PeaksForFit();
}

TPeakFinder::~TPeakFinder(){
  // empty
}

void TPeakFinder::Clear(Option_t *opt){
  fPeakNum = -1;
  fPeaksToFit.clear();
  
}

void TPeakFinder::Print(Option_t *opt) const{
  std::cout << " Printing something " << std::endl;
}

TH1* TPeakFinder::GetBackground(TH1* hist){
  hBkgd = hist->ShowBackground(100,"");
  return hBkgd;
}

TH1* TPeakFinder::GetBgSub(TH1* hist){
  hBkgdSub = (TH1*)hist->Clone(Form("%_bgSub",hist->GetName()));
  hBkgdSub->SetTitle(Form("%_bgSub",hist->GetName()));
  hBkgdSub->Add(this->hBkgd,-1);

  return hBkgdSub;
}

TH1* TPeakFinder::GetScaled(TH1* hist,Double_t cal){
  hScaled = (TH1*)hist->Clone(Form("%s_roughCal",hist->GetName()));
  hScaled->GetXaxis()->Set(this->hBkgdSub->GetNbinsX(),0,cal*this->hBkgdSub->GetXaxis()->GetXmax());

  return hScaled;
}

Bool_t TPeakFinder::PeaksForFit(){

  fPeaksToFit.clear();
  
  std::stringstream ssFile1, ssFile2;
  std::string totalString,nameString,eat;
  ifstream inFile;
  Double_t enHolder = 0.0;
  totalString = std::string(getenv("GRUTSYS"))+"/../libraries/SourceData/";
  ssFile1 << this->GetNucleus()->GetName();
  ssFile1 << "_FitLines.sou";
  ssFile1 >> nameString;
  totalString.append(nameString);
  inFile.open(totalString.c_str());
  if(inFile){
    while(!(inFile.eof())){
      inFile >> enHolder;
      getline(inFile,eat);
      fPeaksToFit.push_back(enHolder);
    }
    std::cout << "> FitLines.sou file loaded with " 
	      << fPeaksToFit.size() << " peaks. "  << std::endl;
  } 
  else{
    for(Int_t i = 0; i < this->GetNucleus()->NTransitions()-1;i++){
      fPeaksToFit.push_back(this->GetNucleus()->GetTransition(i)->GetEnergy());
    }
    std::cout << "> .sou file loaded for source fitting with " 
	      << fPeaksToFit.size() << " peaks. " << std::endl;
  }
  
  fPeakNum = fPeaksToFit.size();
  
  inFile.close();
  return true;
}
