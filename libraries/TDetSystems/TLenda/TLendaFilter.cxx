#include <TLendaFilter.h>

ClassImp(TLendaFilter)


void TLendaFilter::FastFilter(std::vector<UShort_t> &fTrace, std::vector<Double_t> &fFastFilter, int FL, int FG) {
  if(fTrace.size() < 20){
    std::cout << "Event Does not have a valid trace" << std::endl;
    return;
  }

  fFastFilter.clear();
  Double_t sumNum1 = 0;
  Double_t sumNum2 = 0;

  int start = 2 * FL + FG - 1;
  int size = (int)fTrace.size();
  for(int i = 0; i < size; i++){
    if(i >= start) {
      for(int j = (i - (FL - 1)); j <= i; j++) {
	if(j >= 0) sumNum1 += fTrace.at(i);
      }
      for(int j = (i - start); j < (i - (FL+FG)); j++) {
	if(j >= 0) sumNum2 += fTrace.at(i);
      }
    }
    fFastFilter.push_back(sumNum1 - sumNum2);
    sumNum1 = 0;
    sumNum2 = 0;
  }
}

void TLendaFilter::FastFilterOp(std::vector<UShort_t> &fTrace, std::vector<Double_t> &fFastFilter, int FL, int FG) {
  if(fTrace.size() < 20){
    std::cout << "Event Does not have a valid trace" << std::endl;
    return;
  }

  fFastFilter.clear();
  Double_t sumNum1 = 0;
  Double_t sumNum2 = 0;

  int size = fTrace.size();
  int start = size/2 - 0.15*size;
  int end = size/2 + 0.15*size;

  for(int i = start; i < end; i++){
    for(int j = (i - (FL - 1)); j <= i; j++) {
      if(j >= 0) sumNum1 += fTrace.at(i);
    }
    for(int j = (i - (2 * FL + FG -1)); j < (i - (FL + FG)); j++) {
      if(j >= 0) sumNum2 += fTrace.at(i);
    }
    fFastFilter.push_back(sumNum1 - sumNum2);
    sumNum1 = 0;
    sumNum2 = 0;
  }
}

std::vector <Double_t> TLendaFilter::CFD(std::vector<Double_t> &fFastFilter, Double_t CFD_delay, Double_t CFD_scale_factor) {
  std::vector<Double_t> tmpCFD;
  tmpCFD.resize(fFastFilter.size(),0);
  for(int i = 0; i < (int)fFastFilter.size() - CFD_delay; i++) {
    tmpCFD.at(i + CFD_delay) = fFastFilter.at(i + CFD_delay) - fFastFilter.at(i)/(pow(2, CFD_scale_factor + 1));
  }
  return tmpCFD;
}

std::vector <Double_t> TLendaFilter::CFDOp(std::vector<Double_t> &fFastFilter, Double_t CFD_delay, Double_t CFD_scale_factor) {
  std::vector<Double_t> tmpCFD;
  tmpCFD.resize(fFastFilter.size(),0);

  int size = fFastFilter.size();
  int start = size/2 - 0.15*size;
  int end = size/2 + 0.15*size;

  for(int i = start; i < end; i++) {
    tmpCFD.at(i + CFD_delay) = fFastFilter.at(i + CFD_delay) - fFastFilter.at(i)/(pow(2, CFD_scale_factor + 1));
  }
  return tmpCFD;
}

Double_t TLendaFilter::GetZeroCrossing(std::vector<Double_t> &CFD, Int_t &NumZeroCrossings, Double_t &residual) {
  Double_t softwareCFD;
  std::vector <Double_t> ZeroCrossings;
  Double_t MaxValue = 0;
  Int_t MaxIndex = -1;
  int Window = 40;
  for(int i = (((int)CFD.size()/2.0) - Window); i < (((int)CFD.size()/2.0) + Window); i++) {
    if(CFD.at(i) >= 0 && CFD.at(i + 1) < 0 && TMath::Abs(CFD.at(i) - CFD.at(i + 1) > 40)) {//zero crossing point
      softwareCFD = i + CFD.at(i) / ( CFD.at(i) + TMath::Abs(CFD.at(i + 1)) );
      ZeroCrossings.push_back(softwareCFD);
      if(TMath::Abs(CFD.at(i) - CFD.at(i + 1)) > MaxValue) {
        MaxValue = TMath::Abs(CFD.at(i) - CFD.at(i+1));
        MaxIndex = ZeroCrossings.size() - 1;
        residual = CFD.at(i);
      }
    }
  }
  NumZeroCrossings = (int)ZeroCrossings.size();
  if ((int)ZeroCrossings.size() == 0) return sqrt(-1); // no Zero Crossing found
  else return ZeroCrossings[MaxIndex]; // take the max one
}

Double_t TLendaFilter::GetZeroCrossingImprovedRMD(std::vector<Double_t> &CFD, Int_t &MaxInTrace, Double_t &residual) {
  Double_t softwareCFD = 0.0;
  int begin = MaxInTrace;
  int end = (int)CFD.size();

  for(int i = begin; i < end; i++) {
    if(CFD.at(i) >= 0 && CFD.at(i + 1) < 0) {
      softwareCFD = i + CFD.at(i) / ( CFD.at(i) + TMath::Abs(CFD.at(i + 1)) );
      residual = CFD.at(i);
      break;
    }
  }
  return softwareCFD;
}

Double_t TLendaFilter::GetZeroCrossingImproved(std::vector<Double_t> &CFD, Int_t &MaxInTrace, Double_t &residual) {
  Double_t softwareCFD = 0.0;
  int begin = MaxInTrace;
  int end = (int)CFD.size() + 40;

  for(int i = begin; i < end; i++) {
    if(CFD.at(i) >= 0 && CFD.at(i + 1) < 0) {
      softwareCFD = i + CFD.at(i) / ( CFD.at(i) + TMath::Abs(CFD.at(i + 1)) );
      residual = CFD.at(i);
      break;
    }
  }
  return softwareCFD;
}

std::vector <Double_t> TLendaFilter::GetAllZeroCrossings(std::vector<Double_t> &CFD) {
  Double_t softwareCFD;
  std::vector <Double_t> ZeroCrossings;

  for(int i = 0; i< (int) CFD.size()-1; i++) {
    if(CFD.at(i) >= 0 && CFD.at(i + 1) < 0) {
      softwareCFD = i + CFD.at(i) / ( CFD.at(i) + TMath::Abs(CFD.at(i + 1)) );
      ZeroCrossings.push_back(softwareCFD);
    }
  }
  return ZeroCrossings;
}

Double_t TLendaFilter::GetZeroCrossingOp(std::vector <Double_t> & CFD,Int_t & NumZeroCrossings){
  Double_t softwareCFD = -1;
  int size = (int)CFD.size();

  for(int i = (size/2.0 - 20); i < (size/2.0 + 20); i++) {
    if(CFD.at(i) >= 0 && CFD.at(i + 1) < 0 && TMath::Abs(CFD.at(i) - CFD.at(i + 1) > 5))  { //zero crossing point
      softwareCFD = i + CFD.at(i) / ( CFD.at(i) + TMath::Abs(CFD.at(i + 1)) );
      break;
    }
  }
  if (softwareCFD==-1) return sqrt(-1);  // no Zero Crossing found
  else  return softwareCFD;
}

std::vector <Double_t> TLendaFilter::GetZeroCrossingHighRate(const std::vector <Double_t> & CFD,const std::vector<Int_t> &PeakSpots){
  Double_t softwareCFD;
  std::vector <Double_t> ZeroCrossings;
  int size = (int)CFD.size();

  for(auto i : PeakSpots) {
    for(int j = i; j < size - 1; j++) {
      if(CFD.at(j)>= 0 && CFD.at(j + 1) < 0 ) {
        softwareCFD = j + CFD.at(j) / ( CFD.at(j) + TMath::Abs(CFD.at(j + 1)) );
	ZeroCrossings.push_back(softwareCFD);
        j = size + 1000;
	break;
      }
    }
  }
  return ZeroCrossings;
}

std::vector <Double_t> TLendaFilter::GetZeroCrossingCubicHighRate(const std::vector <Double_t> & CFD,const std::vector<Int_t> &PeakSpots){
  Double_t cubicCFD;
  std::vector <Double_t> ZeroCrossings;
  int size = (int)CFD.size();

  for(auto i : PeakSpots) {
    for(int j = i; j < size - 1; j++) {
      if(CFD.at(j)>= 0 && CFD.at(j + 1) < 0 ) {
        cubicCFD = DoMatrixInversionAlgorithm(CFD, j);
	ZeroCrossings.push_back(cubicCFD);
        j = size + 1000;
	break;
      }
    }
  }
  return ZeroCrossings;
}

std::vector<Int_t> TLendaFilter::GetPulseHeightHighRate(const std::vector <UShort_t> & trace,const std::vector<Int_t> &PeakSpots) {
  std::vector<Int_t> result;
  for(auto & i : PeakSpots){
    result.push_back(trace[i]);
  }
  return result;
}


double TLendaFilter::getFunc(TMatrixT<Double_t> Coeffs, double x){
  double total = 0;
  for (int i = 0; i < 4; i++) {
    total = total + Coeffs[i][0]*TMath::Power(x, 3 - i);
  }
  return total;
}

double TLendaFilter::getFunc(std::vector<double> &Coeffs, double x){
  double total = 0;
  for(int i = 0; i < 4; i++){
    total = total + Coeffs[i] * TMath::Power(x, i);
  }
  return total;
}

Double_t TLendaFilter::DoMatrixInversionAlgorithm(const std::vector <Double_t> & CFD, Int_t theSpotAbove){

  Double_t x[4];
  TMatrixT<Double_t> Y(4,1);//a column vector
  for (int i = 0; i < 4; i++) {
    x[i]= theSpotAbove - 1+ i; //first point is the one before zerocrossing
    Y[i][0] = CFD.at(theSpotAbove - 1 + i);
  }

  TMatrixT<Double_t> A(4,4);//declare 4 by 4 matrix
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      A[row][col]= pow(x[row], 3 - col);
    }
  }

  TMatrixT<Double_t> invertA = A.Invert();
  TMatrixT<Double_t> Coeffs(4,1);
  Coeffs = invertA * Y;

  //the x[1] is theSpot above so start there
  bool notDone = true;
  double left = x[1];//initial above
  double right = x[2];//initial below
  double valUp = getFunc(Coeffs,left);
  double valDown = getFunc(Coeffs,right);

  int loopCount=0;
  while(notDone) {
    loopCount++;
    if (TMath::Abs(TMath::Abs(valUp) - TMath::Abs(valDown)) < 0.001) notDone = false;
    double mid = (left+right)/2.0;
    double midVal = getFunc(Coeffs,mid);
    if(midVal > 0) left = mid;
    else right = mid;
    valUp = getFunc(Coeffs,left);
    valDown = getFunc(Coeffs,right);
    if(loopCount > 30){ //kill stuck loop
      notDone=false;
      left = sqrt(-1);
    }
  }
  return left;
}

std::vector <Double_t> TLendaFilter::GetMatrixInversionAlgorithmCoeffients(const std::vector <Double_t> & CFD, Int_t &ReturnSpotAbove) {

  std::map <double,int> zeroCrossings;
  double max = 0;
  int begin = (CFD.size()/2) - 40;
  int end = (CFD.size()/2) + 40;

  for(int i = begin; i < end; i++) {
    if(CFD.at(i) >= 0 && CFD.at(i + 1) < 0) {
      double val = CFD.at(i) - CFD.at(i + 1);
      if(val > max) max = val;
      //put this crossing in map
      zeroCrossings[val] = i;
    }
  }

  int theSpotAbove = zeroCrossings[max];
  ReturnSpotAbove = theSpotAbove;

  Double_t x[4];
  TMatrixT<Double_t> Y(4,1);//a column vector

  for(int i = 0; i < 4; i++){
    x[i]= theSpotAbove - 1 + i; //first point is the one before zerocrossing
    Y[i][0] = CFD.at(theSpotAbove - 1 + i);
  }

  TMatrixT<Double_t> A(4,4);//declare 4 by 4 matrix

  for(int row = 0; row < 4; row++){
    for(int col = 0; col < 4; col++){
      A[row][col]= pow(x[row], 3 - col);
    }
  }

  TMatrixT<Double_t> invertA = A.Invert();
  TMatrixT<Double_t> Coeffs(4,1);
  Coeffs = invertA * Y;
  std::vector <Double_t> retVec;
  for (int i = 0; i < 4; i++){
    retVec.push_back(Coeffs[i][0]);
  }
  return retVec;
}

Double_t TLendaFilter::GetZeroCubicRMD(std::vector<Double_t> &CFD, int &start) {

  int begin = start;
  int end = CFD.size();
  Double_t zero = 0;;
  if (end <= begin) {
    std::cout << "***** ERROR In GetZeroCubicRMD: end of CFD range smaller than beginning" << std::endl;
    return sqrt(-1);
  }

  for(int i = start + 5; i < end; i++) { // Ensure that the starting point is ok
    if((CFD.at(i) > 0) && (CFD.at(i + 1) > 0)) {
      begin = i;
      break;
    }
  }
  for(int i = begin; i < end; i++){ // Ensure that the starting point is ok
    if((CFD.at(i) > 0) && (CFD.at(i + 1) > 0)) {
      zero = DoMatrixInversionAlgorithm(CFD,i);
      return zero;
    }
  }
  return sqrt(-1);
}

Double_t TLendaFilter::GetZeroCubic(std::vector <Double_t> & CFD,Int_t &MaxInTrace){
  int begin = MaxInTrace;
  int end = (CFD.size()/2)+40;
  for (int i = begin; i < end; i++){
    if (CFD.at(i) >= 0 && CFD.at(i+1) < 0){
      return DoMatrixInversionAlgorithm(CFD, i);
    }
  }
  return sqrt(-1);
}

Double_t TLendaFilter::GetZeroFitCubic(std::vector <Double_t> & CFD){
  //Find the largest zero Crossing
  std::map <double,int> zeroCrossings;
  double max = 0;

  //restrict search to center part of tace
  int begin = (CFD.size()/2) - 40;
  int end = (CFD.size()/2) + 40;

  for(int i = begin; i < end;i++) {
    if(CFD.at(i) >= 0 && CFD.at(i + 1) <0) {
      double val = CFD.at(i) - CFD.at(i + 1);
      if(val > max) max = val;
      //put this crossing in map
      zeroCrossings[val]=i;
    }
  }
  //Take the spot before out of map
  int theSpotAbove = zeroCrossings[max];

  //Points to consider are { spotAbove-2,spotAbove-1,
  //spotAbove, spotBelow, spotBelow+1,spotBelow+2}
  // Use fitting to find 3rd order polynomial from these 6 points

  Double_t x[6];
  Double_t y[6];
  for (int i = 0; i < 6; i++){
    x[i] = theSpotAbove - 2+ i; //first point is the one before zerocrossing
    y[i] = CFD.at(theSpotAbove -2 + i);
  }
  TGraph theGraphForFitting(6,x,y);
  TFitResultPtr fitPointer =  theGraphForFitting.Fit("pol3","QSN");
  Int_t status = fitPointer;
  if (status == 0) { // if the Fit was a success
    std::vector<double> Coeffs(4);//3rd order polynomial should have 4 coeffs
    Coeffs[0]=fitPointer->Value(0);
    Coeffs[1]=fitPointer->Value(1);
    Coeffs[2]=fitPointer->Value(2);
    Coeffs[3]=fitPointer->Value(3);

    //the x[1] is theSpot above so start there
    bool notDone = true;
    double left = x[2];//initial above should be in 3rd spot
    double right = x[3];//initial below should be in 4th spot
    double valUp = getFunc(Coeffs,left);
    double valDown = getFunc(Coeffs,right);
    int loopCount=0;
    while (notDone){
      loopCount++;
      if (TMath::Abs(TMath::Abs(valUp) - TMath::Abs(valDown)) < 0.001) notDone = false;
      double mid = (left + right)/2.0;
      double midVal = getFunc(Coeffs, mid);
      if (midVal > 0) left = mid;
      else right = mid;
      valUp = getFunc(Coeffs, left);
      valDown =getFunc(Coeffs, right);
      if (loopCount >30 ){//kill stuck loop
        notDone = false;
        left = sqrt(-1);
      }
    }
    return left;
  } else {// End Fit Status If
    std::cout<< "FIT FAILED" << std::endl;
    return sqrt(-1);
  }
}

Double_t TLendaFilter::fitTrace(std::vector <UShort_t> & trace,Double_t sigma,Double_t num) {

  Int_t size = (Int_t) trace.size();
  std::vector <Double_t> y_values,x_values;

  for ( Int_t i = 0; i < size; i++){
    y_values.push_back( (double_t) trace.at(i)  );
    x_values.push_back( (Double_t) i);
  }

  //Find Maximum
  Double_t max = -1;
  Int_t maxBin = -1;
  for(Int_t i = 0; i < size; i++){
    if(y_values[i] > max){
      max = y_values[i];
      maxBin = i;
    }
  }
  //Set up fit function
  Int_t fitWindowWidth = 10;  //plus or minus 5 bins on either side of max
  //to be taken into acount during fit
  Double_t mu = -1000;

  //1.7711 the sigma determined from looking at traces
  TF1 *myfit = new TF1("myfit", "gaus(0) + [3]", 0, 200);
  myfit->SetParameter(0, 1);
  myfit->SetParameter(1, 100);
  myfit->FixParameter(2, sigma);
  myfit->SetParameter(3, 300);
  myfit->SetParLimits(0, 0, 1000);// Make sure the constant out in front is>0

  //Define the trace as a Tgraph
  TGraph * gr = new TGraph(size, x_values.data(), y_values.data());
  TFitResultPtr fitPointer = gr->Fit("myfit", "S0Q", "", maxBin - fitWindowWidth, maxBin + fitWindowWidth);

  Int_t fitStatus = fitPointer;
  if ( fitStatus == 0 ) { //no errors in fit
    mu = fitPointer->Value(1);
  } else {
    numOfBadFits++;
  }
  //Delete objects
  gr->Delete();
  myfit->Delete();
  return mu;
}

Double_t TLendaFilter::GetEnergy(std::vector <UShort_t> &trace,Int_t MaxSpot) {

  ////////////////////////////////////////////////////////////////////////////
  // Slightly more complicated algorithm for getting energy of pulse.       //
  // Takes the first 20% of the trace as a background.  Then it takes only  //
  // a window around the maximum spot as the pulse part of the trace.  This //
  // helps with strange pulse where there might be additional structures in //
  // the trace that would distort things (afterpulse, crazy noise..)        //
  ////////////////////////////////////////////////////////////////////////////

  Double_t thisEventsIntegral;
  Double_t sumBegin = 0;
  Double_t sumEnd = 0;

  Double_t signalIntegral=0;

  int traceLength = trace.size();
  int LengthForBackGround = 0.2 * traceLength;

  Int_t windowForEnergy = 20;

  if( MaxSpot - windowForEnergy < 0 ) {
    return sqrt(-1);
  } else if (MaxSpot + windowForEnergy > (traceLength -1)) {
    return sqrt(-1);
  }

  for ( int i = 0 ; i < LengthForBackGround; i++){
    sumBegin = sumBegin + trace.at(i);
    sumEnd = sumEnd + trace.at(traceLength - 1 - i);
  }

  Double_t BackGround;

  if (MaxSpot > LengthForBackGround && MaxSpot < (traceLength - LengthForBackGround)) {
    //The peak is in the middle use the beginning of trace as background
    BackGround = sumBegin/LengthForBackGround;
  } else if (MaxSpot < LengthForBackGround) {
    //The peak is in the beginning of trace use end as background
    BackGround = sumEnd/LengthForBackGround;
  } else if (MaxSpot > (traceLength - LengthForBackGround)) {
    //Peak is at end use beginning for backaground
    BackGround = sumBegin/LengthForBackGround;
  } else{
    //Something makes no sense
    return sqrt(-1);
  }

  for (int i = MaxSpot - windowForEnergy; i < MaxSpot + windowForEnergy; i++) {
    signalIntegral = trace.at(i) + signalIntegral;
  }

  if(signalIntegral - BackGround * (2 * windowForEnergy) > 0) {
    thisEventsIntegral = signalIntegral - BackGround *(2 * windowForEnergy);
  } else {
    thisEventsIntegral = sqrt(-1);
  }
  return thisEventsIntegral;
}

Double_t TLendaFilter::GetEnergyRMD(std::vector <UShort_t> &trace,Int_t MaxSpot){ // JP for RMD 2017 campaign
  // Based on GetEnergy but with asymmetric windows. The window after the maximum
  // is now higher to make sure that we get
  // most of the pulse in the tail. I'm using window=100 instead of 20 (LENDA). This
  // is only valid for the new RMD sample (the one used in 2016 had huge tails).
  // Window = 70 may still miss part of the tail for neutrons, but I don't want
  // to be too far, so I still avoid additional structures in
  // the trace that would distort things (afterpulse, crazy noise..)

  Double_t thisEventsIntegral;
  Double_t sumBegin = 0;
  Double_t sumEnd = 0;

  Double_t signalIntegral=0;
  int traceLength=trace.size();
  int LengthForBackGround = 0.2 * traceLength;

  Int_t windowForEnergyleft = 10;
  Int_t windowForEnergyright = 70;

  if(MaxSpot - windowForEnergyleft < 0){
    return sqrt(-1);
  } else if(MaxSpot + windowForEnergyright > (traceLength - 1) ) {
    return sqrt(-1);
  }

  for(int i = 0; i < LengthForBackGround; i++) {
    sumBegin = sumBegin + trace.at(i);
    sumEnd = sumEnd + trace.at(traceLength - 1 - i);
  }

  Double_t BackGround;
  if (MaxSpot > LengthForBackGround && MaxSpot < (traceLength - LengthForBackGround)) {
    //The peak is in the middle use the beginning of trace as background
    BackGround = sumBegin/LengthForBackGround;
  } else if(MaxSpot < LengthForBackGround) {
    //The peak is in the beginning of trace use end as background
    BackGround = sumEnd/LengthForBackGround;
  } else if(MaxSpot > (traceLength - LengthForBackGround)) {
    //Peak is at end use beginning for backaground
    BackGround = sumBegin/LengthForBackGround;
  } else{
    //Something makes no sense
    return sqrt(-1);
  }


  for(int i = MaxSpot - windowForEnergyleft; i < MaxSpot + windowForEnergyright; i++) {
    signalIntegral = trace.at(i)+ signalIntegral;
  }

  if(signalIntegral - BackGround *(windowForEnergyleft + windowForEnergyright) > 0) {
    thisEventsIntegral = signalIntegral - BackGround * (windowForEnergyleft + windowForEnergyright);
  } else {
    thisEventsIntegral = sqrt(-1);
  }
  return thisEventsIntegral;
}

std::vector<Double_t> TLendaFilter::GetEnergyHighRate(const std::vector <UShort_t> & trace,std::vector <Int_t> &PeakSpots, std::vector<Double_t>& theUnderShoots, Double_t & MaxValueOut,Int_t & MaxIndexOut) {
  Int_t NumberOfSamples = 8;
  Int_t TraceLength = trace.size();

  //Each chuck of Sample size trace points is used to calculate the point in the trace with
  //the most stable baseline
  Int_t SampleSize = TMath::Floor(TraceLength/Float_t(NumberOfSamples));

  std::vector<Double_t> StandardDeviations(NumberOfSamples,0);
  std::vector<Double_t> Averages(NumberOfSamples,0);

  //First For each sample window find the average of those points and find
  //the Maximum point (will exclude the reamining points at the end of the trace
  Double_t Max = 0;
  Int_t MaxIndex = -1;
  for(int i = 0; i < NumberOfSamples; i++){//I loops over the samples
    for(int j = 0; j < SampleSize; j++){//J Loops over the points in each sample
      Double_t val = trace.at(j + i * SampleSize);
      Averages[i] += val;
      if (val > Max){
        Max = val;
        MaxIndex = j + i * SampleSize;
      }
    }//end for over j
    Averages[i]/=SampleSize;//Now that the points in the sample have been summed take average
  }//end over i

  ///Now look at the end of the trace to see if the maximum is there
  for (int i= SampleSize * NumberOfSamples; i < TraceLength; i++){
    Double_t val = trace.at(i);
    if (val > Max){
      Max = val;
      MaxIndex = i;
    }
  }
  //Copy over values to output variables
  MaxValueOut = Max;
  MaxIndexOut = MaxIndex;

  //Find stardard deviation for each base line chuck
  std::map<Double_t,std::pair<int,int> > baseLineRanges;
  std::map<Double_t,Double_t> StandardDeviation2Average;
  for(int i = 0; i < NumberOfSamples; i++){
    for(int j = 0; j < SampleSize; j++){
      StandardDeviations[i] += TMath::Power((trace.at(j + i * SampleSize) - Averages.at(i)), 2);
    }

    baseLineRanges.insert(std::make_pair(StandardDeviations.at(i), std::make_pair(i * SampleSize, ( i + 1) * SampleSize)));
    StandardDeviation2Average.insert(std::make_pair(StandardDeviations.at(i), Averages.at(i)));
  }

  //Take beginning of map which will give the smallest stardard deviation
  auto it = StandardDeviation2Average.begin();
  //The best baseline will be the average of the chuck with the smallest standard deviation
  Double_t BaseLine = it->second;

  //Now we have found the base line and the maximum point.  We look for other real pulses in trace
  //Threshold should be the amount above baseline.
  Int_t  ThresholdForOtherPulseInTrace= MaxValueOut*0.15;

  Int_t windowForEnergy = 2;
  Int_t EndTraceCut = 2;
  Double_t currentMax = 0;
  bool HasCrossedThreshold = false;

  //Loop through the Trace and try and find the peak values
  //Exclude the first point.  It cannot be a maximum
  for(int i = 1; i < TraceLength - EndTraceCut; i++) {
    Double_t val = trace.at(i) - BaseLine;
    //Check to see if this value has gone above the threshold
    if(val > ThresholdForOtherPulseInTrace){
      HasCrossedThreshold = true;
    }
    if(val > currentMax){//increase the maximum
      currentMax = val;
      //the trace value is less than the previous point (possible peak)
    } else if(val < currentMax && HasCrossedThreshold){
      //Possible peak spot
      int PossibleSpot = i - 1;//The peaks spot will be the point before this one (the current i)
      //check to see that spot before/after peak is less than peak value
      if(trace.at(PossibleSpot - 1) < trace.at(PossibleSpot) && trace.at(PossibleSpot + 1) < trace.at(PossibleSpot)) {
        PeakSpots.push_back(PossibleSpot);
        //reset the current max value
        currentMax = 0;
        HasCrossedThreshold = false;
        //Now must skip foward in time  around the length of one RF bucket
        //IE the minimum amount of time before a second real pulse could be there
        int index = i + 8;//(+8 because we are at i+1 and will get +1 again at end of loop) 10 clock tics is 40 nano$
        if(index > TraceLength - windowForEnergy) {
          //too close to end of trace.  End the search
          i = TraceLength + 1000;
          break;
        } else {
          //Skip foward to place
          i = index;
        }//end else
      }//end confirmed peak spot
    }//end possible peak spot
  }//end for


  //The Maximums have been found
  //Now finally we calculate pulse integrals
  std::vector <Double_t> theEnergies;
  for(auto i : PeakSpots){
    Double_t temp = 0;
    for (int j = 0; j < 2; j++){
      temp += (trace.at(i + j) - BaseLine);
    }
    theEnergies.push_back(temp);
    Double_t min = 100000000.0;
    for(int j = 0; j < 6; j++) {
      if(trace.at(i - j) < min) {
        min = trace.at(i - j);
      }
    }
    double cut = 0.94 * BaseLine;
    if (min < cut){
      theUnderShoots.push_back(BaseLine - min);
    } else{
      theUnderShoots.push_back(sqrt(-1));
    }
  }
  return theEnergies;
}

Double_t TLendaFilter::GetGateRMD(std::vector <UShort_t> &trace, int &start, int L) {

  int range = L;
  int step = 3;
  int size = trace.size();
  int _start = -1;

  Double_t tr1, tr2, diff;
  Double_t maxdiff = 0;
  start = 0;

  for(int j = 0; j < size - step; j++){
    tr1 = trace.at(j);
    tr2 = trace.at(j + step);
    diff = tr2 - tr1;
    if (diff > maxdiff) {
      maxdiff = diff;
      _start = j;
    }
  }

  start = _start;
  int window = floor( trace.size()/5.0);
  Double_t bgk=0;

  for(int i = 0; i < window; i++){
    bgk = trace.at(i) + bgk;
  }
  bgk = bgk/(window);
  Double_t total = 0;
  for (int i = start; i < start + L; i++) total = total + trace.at(i);
  return total - (bgk * range);
}

Double_t TLendaFilter::GetGate(std::vector <UShort_t> &trace, int start, int L) {

  int range = L;
  int window = floor( trace.size()/5.0);
  Double_t bgk = 0;

  for(int i = 0; i < window; i++){
    bgk = trace.at(i) + bgk;
  }
  bgk=bgk/(window);
  Double_t total = 0;
  for (int i = start; i < start + L; i++) total = total + trace.at(i);
  return total - (bgk * range);
}


Int_t TLendaFilter::GetMaxPulseHeight(std::vector <UShort_t> &trace, Int_t& MaxSpot) {

  int _maxSpot = -1;
  Double_t max = 0;
  for(int i = 0; i < (int)trace.size(); i++){
    if(trace.at(i) > max){
      max = trace[i];
      _maxSpot = i;
    }
  }
  MaxSpot = _maxSpot;
  return max;
}


Int_t TLendaFilter::GetMaxPulseHeight(std::vector <Double_t> &trace, Int_t& MaxSpot) {

  int _maxSpot = -1;
  Double_t max = 0;
  for (int i = 0; i < (int)trace.size(); i++) {
    if(trace.at(i) > max){
      max = trace.at(i);
      _maxSpot = i;
    }
  }
  MaxSpot = _maxSpot;
  return max;
}

// Extract time from Leading Edge
Double_t TLendaFilter::GetLeadingEdge(std::vector <UShort_t> &trace) {

  int traceLength = trace.size();
  bool foundLE = false;
  Double_t letime = sqrt(-1);

  /////////////////////////////////////////////////
  // Find maximum of trace and pulse height
  /////////////////////////////////////////////////
  Int_t maxSpot = -1;
  Double_t tmax = 0;
  for (int i = 0; i < traceLength; i++) {
    if(trace.at(i) > tmax){
      tmax = trace.at(i);
      maxSpot = i;
    }
  }
  /////////////////////////////////////////////////
  // Calculate baseline (background) and noise
  /////////////////////////////////////////////////
  Double_t sumBackground;
  Double_t background = -1;
  int LengthForBackground = 0.2 * traceLength;

  UShort_t y;
  if(maxSpot < LengthForBackground) { // Make sure that pulse maximum is not in the baseline region
    background = sqrt(-1);
    return letime;
  } else{ // We have a "good" baseline region. Extract information
    sumBackground = 0;
    for(int j = 0; j < LengthForBackground; j++) { // Scan trace
      y = trace.at(j);
      sumBackground = sumBackground + y;
    }
    background = sumBackground / LengthForBackground; // Calculate background
  }
  /////////////////////////////
  // Search now for LE
  /////////////////////////////
  Double_t LeadingEdge = background + 100; // Define leading-edge over baseline
  int j = 0;
  while(j < traceLength - 1 && !foundLE) {
    if (trace.at(j) < LeadingEdge && trace.at(j+1) > LeadingEdge) {
      letime = j + (LeadingEdge - trace.at(j)) / (trace.at(j + 1) - trace.at(j));
      foundLE = true;
    } else {
      j++;
    }
  }
  if (!foundLE) letime = sqrt(-1);
  return letime;
}

Double_t TLendaFilter::GetFilterEnergy(std::vector <UShort_t> &trace){
  // This function uses the trace to calculate the detector energy using the trapezoidal filter
  // described by Tan et al., IEEE trans. of nucl. sci., 51, 1541 (2004) (JP 4/3/2018)

  Double_t At = sqrt(-1);

  Double_t L = 30;    // Filter length
  Double_t G;         // Filter Gap length
  Double_t tau = 25;  // decay time
  Int_t step = 3;     // Differential step to calculate inflextion point where pulse starts

  Int_t _maxSpot = -1;
  Double_t _background = -1;
  int traceLength = trace.size();

  /////////////////////////////////////////////////
  // Find maximum of trace and pulse height
  /////////////////////////////////////////////////
  Double_t tmax = 0;
  for(int i = 0; i < traceLength; i++){
    if(trace.at(i) > tmax){
      tmax = trace.at(i);
      _maxSpot = i;
    }
  }

  /////////////////////////////////////////////////
  // Calculate baseline (background) and noise
  /////////////////////////////////////////////////
  Double_t sumBackground;
  int LengthForBackground = 0.2 * traceLength;

  if(_maxSpot < LengthForBackground) { // Make sure that pulse maximum is not in the "noise" baseline region
    _background = sqrt(-1);
    return At;
  } else { // We have a "good" baseline region. Extract information
    sumBackground = 0;
    for(int j = 0 ; j < LengthForBackground; j++) { // Scan trace
      sumBackground += trace.at(j);
    }
    _background = sumBackground / LengthForBackground; // Calculate background
  }

  /////////////////////////////////////////////////
  // Calculate rising time
  /////////////////////////////////////////////////
  Double_t tr1, tr2, diff = -1;
  Double_t maxdiff = 0;

  for(int k = 0; k < traceLength - step; k++){
    tr1 = trace.at(k);
    tr2 = trace.at(k + step);
    diff = tr2 - tr1;
    if(diff > maxdiff) {
      maxdiff = diff;
    }
  }

  /////////////////////////////////////////////////
  // Calculate energy from filter
  /////////////////////////////////////////////////
  int S0begin, S0end, Sgbegin, Sgend, S1begin, S1end;
  Double_t S0, Sg, S1, a0, ag, a1;
  Double_t b, bL;

  G = 0.5 * L;
  b = exp (-1/tau);
  bL = pow(b,L);
  a1 = 1 / (1 - bL);
  a0 = -bL * a1;
  ag = 1;

  Sgbegin = _maxSpot - G/2;
  Sgend = Sgbegin + G -1;
  S0begin = Sgbegin - L;
  S0end = Sgbegin - 1;
  S1begin = Sgend + 1;
  S1end = S1begin + L - 1;

  if(S0begin > 0 && S1end < traceLength) {
    S0 = 0;
    Sg = 0;
    S1 = 0;
    for (int i=0; i<traceLength; i++) {
      if (i >= S0begin && i <= S0end) S0 += trace[i];
      if (i >= Sgbegin && i <= Sgend) Sg += trace[i];
      if (i >= S1begin && i <= S1end) S1 += trace[i];
    }
    At = a0*S0 + ag*Sg + a1*S1 - _background*(L+G);
  } else {
    At = sqrt(-1);
  }
  return At;
}

Double_t TLendaFilter::GetMaxPulseHeightBaseline(std::vector <UShort_t> &trace,Int_t& MaxSpot) { // Pass MaxSpot as reference

  // This function calculates the pulse height and subtracts the baseline. JP 5/17/2016
  // Slightly corrected to calculate position of maximum before comparing it with LengthForBackGround
  // That was an error in the previous version (2016 campaign) that didn't blow things out because
  // MaxSpot had been already determined in GetMaxPulseHeight, called from LendaPacker::CalcAll, JP 3/6/2017
  // I also pass MaxSpot as reference instead of variable to make sure that the newly created MaxSpot
  // is passed to LendaPacker::CalcAll, which will need it for other filter functions

  // Calculate baseline
  Double_t sumBegin = 0;
  int traceLength = trace.size();
  int LengthForBackGround = 0.2 * traceLength;

  for(int i = 0; i < LengthForBackGround; i++) {
    sumBegin = sumBegin + trace.at(i);
  }

  Double_t BackGround = -1;

  // Now find maximum of trace like in function GetMaxPulseHeight
  int _maxSpot = -1;
  Double_t max = 0;
  Double_t max_base = 0;
  for(int i = 0; i < traceLength; i++) {
    if(trace.at(i) > max) {
      max = trace.at(i);
      _maxSpot = i;
    }
  }
  MaxSpot = _maxSpot;

  // Now, with a fresh calculated MaxSpot, check that it happens after LengthForBackground, JP 3/6/2017
  if(MaxSpot > LengthForBackGround) {
    BackGround = sumBegin/LengthForBackGround;
  } else {
    //Maximum of trace is in the "background" area
    return sqrt(-1);
  }
  // Finally, subtract baseline
  if (BackGround != -1) {
    max_base = max - BackGround;
  } else {
    max_base = sqrt(-1);
  }
  return max_base;
}

Double_t TLendaFilter::GetPulseComplete(std::vector <UShort_t> &trace, Int_t &MaxSpot, Double_t &rising, Double_t &background, Double_t &noise) {
  // This function extracts several parameters characterizing the pulse shape (JP 5/20/2016) returns
  // the pulse-height minus the baseline, It also calculates: maximum, rising time, baseline, and noise level
  Int_t _maxSpot =-1;
  Double_t _noise = -1;
  Double_t _background = -1;
  Double_t _rising = -1;
  Double_t max_base = -1;

  /////////////////////////////////////////////////
  // Find maximum of trace and pulse height
  /////////////////////////////////////////////////
  Double_t tmax = 0;
  int traceLength = trace.size();
  for(int i = 0; i < traceLength; i++) {
    if(trace.at(i) > tmax) {
      tmax = trace.at(i);
      _maxSpot = i;
    }
  }

  /////////////////////////////////////////////////
  // Calculate baseline (background) and noise
  /////////////////////////////////////////////////
  Double_t sumBackground;
  int LengthForBackground = 0.2 * traceLength;

  UShort_t y;
  Double_t max, min, sigma2;

  if(_maxSpot < LengthForBackground) { // Make sure that pulse maximum is not in the "noise" baseline region
    std::cout << "******* WARNING: Pulse max too early (in the baseline region) !!!" << std::endl;
    _noise = sqrt(-1);
    _background = sqrt(-1);
  } else { // We have a "good" baseline region. Extract information
    max = -1000;
    min = 1000;
    sumBackground = 0;
    for(int j = 0; j < LengthForBackground; j++) { // Scan trace
      y = trace.at(j);
      // Search for maximum and minimun signals withing "noise" baseline region
      if(y > max) {
        max = y;
      } else if (y < min) {
        min = y;
      }
      // Integrate background
      sumBackground = sumBackground + y;
    }
    _background = sumBackground / LengthForBackground; // Calculate background
    // Now determine the standard deviation in the baseline region (this is the noise)
    sigma2 = 0;
    for(int j = 0; j < LengthForBackground; j++) { // Scan trace
      sigma2 = sigma2 + pow((trace.at(j) - _background), 2);
    }
    _noise = sqrt(sigma2/LengthForBackground);
  }
  /////////////////////////////////////////////////
  // Calculate rising time
  /////////////////////////////////////////////////
  Int_t start = 0;
  Int_t step = 3;
  Double_t tr1, tr2, diff = -1;
  Double_t maxdiff = 0;
  for(int k = 0; k < traceLength - step; k++) {
    tr1 = trace.at(k);
    tr2 = trace.at(k + step);
    diff = tr2 - tr1;
    if(diff > maxdiff) {
      maxdiff = diff;
      start = k;
    }
  }
  if(maxdiff > 0 && _maxSpot > 0) {
    _rising = _maxSpot - start;
  } else {
    _rising = sqrt(-1);
  }

  /////////////////////////////////////////////////
  // Calculate pulse-height minus baseline
  /////////////////////////////////////////////////
  if(_background != -1) {
    max_base = tmax - _background;
  } else {
    max_base = sqrt(-1);
  }
  MaxSpot = _maxSpot;
  rising = _rising;
  background = _background;
  noise = _noise;
  return max_base;
}

Int_t TLendaFilter::GetStartForPulseShape(Int_t MaxSpot){
  if(MaxSpot - 4 > 0){
    return (MaxSpot - 4);
  } else {
    return 0;
  }
}

Int_t TLendaFilter::CalculateCFD(std::vector<UShort_t> trace, std::vector<Double_t> &CFDOut) {
  CFDOut.clear();
  Int_t TFLength = 6;
  Int_t TFGap = 0;
  Int_t CFDSF = 4;
  Int_t CFDDelay = 6;
  Double_t CFDWeight = 0.5;

  Int_t A = 0;
  Int_t m = 0;

  Double_t S0 = 0;
  Double_t S1 = 0;
  Double_t S2 = 0;
  Double_t S3 = 0;

  if(TFLength < 0) {
    std::cout << "Set the Trigger Filter Length!" << std::endl;
    return -107;
  }
  if(TFGap < 0) {
    std::cout << "Set the Trigger Filter Gap!" << std::endl;
    return -108;
  }
  if(CFDSF < 0) {
    std::cout << "Set CFD Scale Factor!" << std::endl;
    return -115;
  }
  if(CFDDelay < 0) {
    std::cout << "Set CFD Delay!" << std::endl;
    return -116;
  }

  if(trace.size() > 0) {
    //Calculate the CFD Response
    for(m = 0; m < (CFDDelay + TFLength + TFGap + 1); m++) {
      CFDOut.push_back(0);
    }
    m = CFDDelay + 2 * TFLength + TFGap;

    for(A = (m - CFDDelay - 2 * TFLength - TFGap); A < (m - CFDDelay - TFGap - TFLength); A++) {
      S3 += trace.at(A);
    }
    for(A = (m - CFDDelay - TFLength); A < (m - CFDDelay); A++) {
      S2 += trace.at(A);
    }
    for(A = (m - 2 * TFLength - TFGap); A<(m - TFGap - TFLength); A++) {
      S1 += trace.at(A);
    }
    for(A = m - TFLength; A < m; A++) {
      S0 += trace.at(A);
    }
    for(m = (CFDDelay + 2 * TFLength + TFGap + 1); m < (Int_t)(trace.size()); m++) {
      S3 = S3 - trace.at(m - CFDDelay - 2 * TFLength - TFGap - 1) + trace.at(m - CFDDelay - TFGap - TFLength - 1);
      S2 = S2 - trace.at(m - CFDDelay - TFLength-1) + trace.at(m-CFDDelay-1);
      S1 = S1 - trace.at(m - 2 * TFLength - TFGap - 1) + trace.at(m-TFGap-TFLength-1);
      S0 = S0 - trace.at(m - TFLength - 1)+trace[m-1];
      Double_t cfdvalue2 = CFDWeight * (S0 - S1) - (S2 - S3);
      CFDOut.push_back(cfdvalue2);
    }
  } else {
    std::cout << "Trace is length 0!" << std::endl;
    return -105;
  }
  return 0;
}

std::vector <Double_t> TLendaFilter::GetNewFirmwareCFD(const std::vector<UShort_t> &trace, Int_t FL, Int_t FG, Int_t d, Int_t w) {
  std::vector<Double_t> CFDOut;
  if(trace.size() == 0) {
    CFDOut.resize(1,sqrt(-1));//Return a BAD CFD!
    return CFDOut;
  }

  Int_t TFLength = FL;//FL;
  Int_t TFGap = FG;//FG;
  Int_t CFDDelay = d;
  Double_t CFDWeight = GetNewFirmwareCFDWeight(w);
  Int_t A = 0;
  Int_t m = 0;

  ///////////////////////////////////////////
  // There are 4 sums in the CFD algorithm //
  ///////////////////////////////////////////
  Double_t S0 = 0;
  Double_t S1 = 0;
  Double_t S2 = 0;
  Double_t S3 = 0;

  for(m = 0; m < (CFDDelay + 2 * TFLength + TFGap + 1); m++) {
    CFDOut.push_back(0);
  }
  m = CFDDelay + 2 * TFLength + TFGap;
  for(A = (m - CFDDelay - 2 * TFLength - TFGap); A < (m - CFDDelay - TFGap - TFLength); A++) {
    S3 += trace.at(A);
  }
  for(A = (m - CFDDelay - TFLength); A < (m - CFDDelay); A++) {
    S2 += trace.at(A);
  }
  for(A = (m - 2 * TFLength - TFGap); A < (m - TFGap - TFLength); A++) {
    S1 += trace.at(A);
  }
  for(A = m - TFLength; A < m; A++) {
    S0 += trace.at(A);
  }
  for(m=(CFDDelay+2*TFLength+TFGap+1); m<(Int_t)(trace.size()); m++) {
    S3 = S3 - trace.at(m - CFDDelay - 2 * TFLength - TFGap - 1) + trace.at(m - CFDDelay - TFGap - TFLength - 1);
    S2 = S2 - trace.at(m - CFDDelay - TFLength - 1) + trace.at(m - CFDDelay - 1);
    S1 = S1 - trace.at(m - 2*TFLength - TFGap - 1) + trace.at(m - TFGap - TFLength - 1);
    S0 = S0 - trace.at(m - TFLength - 1) + trace.at(m - 1);
    Double_t cfdvalue = CFDWeight*(S0 - S1) - (S2 - S3);
    CFDOut.push_back(cfdvalue);
  }
  return CFDOut;
}

Double_t TLendaFilter::GetNewFirmwareCFDWeight(Int_t CFDScaleFactor){
  Double_t temp = (Double_t) CFDScaleFactor;
  return (8.0 - temp)/8.0;
}

