

#include <GH1.h>
#include <GH2.h>

#include <TF1.h>
//#include <Math/ParamFunctor.h>


GH1D *GH1::Project(int low,int high) {
  if(!GetParent() || !GetParent()->InheritsFrom(GH2::Class()))
    return 0;
  GH2 *p = (GH2*)GetParent();
  if(fProjectionAxis==kNoAxis)
    return 0;
  switch(fProjectionAxis) {
    case kXaxis:
      return p->ProjectionY("_py",low,high,"keep+");      
    case kYaxis:
      return p->ProjectionX("_px",low,high,"keep+");
  };
  return 0;
}

GH1D *GH1::Project(int low,int high,int bg_low,int bg_high,double scale) {
  if(!GetParent() || !GetParent()->InheritsFrom(GH2::Class()))
    return 0;
  GH2 *p = (GH2*)GetParent();
  if(fProjectionAxis==kNoAxis)
    return 0;
  switch(fProjectionAxis) {
    case kXaxis:
      return p->ProjectionY_BG("_bg_py",low,high,bg_low,bg_high,scale,"keep+");      
    case kYaxis:
      return p->ProjectionX_BG("_bg_px",low,high,bg_low,bg_high,scale,"keep+");
  };
  return 0;
}





double GH1::FitEval(double *dim,double *par) {

  double x = dim[0];
  //printf("par = 0x%08x\n",par); fflush(stdout);
  double scale = par[0];

  int binNum = GetXaxis()->FindBin(x); //gHist->GetBin() does not respect rebinning.

  int nBins = GetNbinsX();
  int kevPerBin = GetXaxis()->GetXmax()/nBins;
  int curBinX = GetBinCenter(binNum);
  int nextBinX = GetBinCenter(binNum+1);
  int prevBinX = GetBinCenter(binNum-1);

  if (x > prevBinX && x <= curBinX){
    double leftDiff = x - prevBinX;
    double rightDiff = curBinX - x;

    leftDiff = 1.0 - leftDiff/(double)kevPerBin;   //These numbers are now less than 1
    rightDiff = 1.0 - rightDiff/(double)kevPerBin; //and a measure of how close it is to that bin
    double binContentLeft = GetBinContent(binNum-1);
    double binContentRight = GetBinContent(binNum);
    return scale * (leftDiff*binContentLeft+rightDiff*binContentRight);
  }

  else if (x > curBinX && x < nextBinX){
    double leftDiff = x - curBinX;
    double rightDiff = nextBinX - x;

    leftDiff = 1.0 - leftDiff/(double)kevPerBin;
    rightDiff = 1.0 - rightDiff/(double)kevPerBin;
    double binContentLeft = GetBinContent(binNum);
    double binContentRight = GetBinContent(binNum+1);
    return scale * (leftDiff*binContentLeft+rightDiff*binContentRight);
  }
  //std::cout << "FAILED IN HISTVALUE!" << std::endl;
  return scale * GetBinContent(binNum);
}


TF1  *GH1::ConstructTF1() const {
  if(GetDimension()!=1)
    return 0;

  ROOT::Math::ParamFunctor *f = new  ROOT::Math::ParamFunctor((GH1*)this,&GH1::FitEval);
  double low  = GetXaxis()->GetBinLowEdge(1);
  double high = GetXaxis()->GetBinUpEdge(GetXaxis()->GetNbins());
  
  TF1 *tf1 = new TF1(Form("%s_tf1",GetName()),*f,low,high,1,1);
  tf1->SetParameter(0,1.0);
  tf1->SetNpx(GetXaxis()->GetNbins());  
  return tf1;
 
}
















