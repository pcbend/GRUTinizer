#ifndef _TLENDAFILTER_H_
#define _TLENDAFILTER_H_

#include <TVector3.h>
#include <TDetectorHit.h>
#include <TGraph.h>
#include <TF1.h>
#include <TFitResult.h>

class TLendaFilter : public TDetectorHit {
  public:
    TLendaFilter() { }
    ~TLendaFilter() { }

    void FastFilter(std::vector <UShort_t> &fTrace, std::vector <Double_t> &fFastFilter, int FL, int FG);
    void FastFilterOp(std::vector <UShort_t> &fTrace, std::vector <Double_t> &fFastFilter, int FL, int FG);

    std::vector<Double_t> CFD(std::vector <Double_t> &fFastFilter, Double_t CFD_delay, Double_t CFD_scale_factor);
    std::vector<Double_t> CFDOp(std::vector <Double_t> &fFastFilter, Double_t CFD_delay, Double_t CFD_scale_factor);

    Double_t GetZeroCrossing(std::vector <Double_t> &CFDVec, Int_t &NZero, Double_t &resid);
    Double_t GetZeroCrossingImprovedRMD(std::vector <Double_t> &CFDVec, Int_t &MaxTrace, Double_t &resid);
    Double_t GetZeroCrossingImproved(std::vector <Double_t> &CFDVec,Int_t &MaxTrace, Double_t &resid);
    Double_t GetZeroCrossingOp(std::vector <Double_t> &CFDVec, Int_t &NCross);

    std::vector<Double_t> GetAllZeroCrossings(std::vector <Double_t> &CFDVec);
    std::vector<Double_t> GetZeroCrossingHighRate(const std::vector<Double_t> & CFD,const std::vector<Int_t> & PeakSpots);
    std::vector<Double_t> GetZeroCrossingCubicHighRate(const std::vector<Double_t> & CFD,const std::vector<Int_t> & PeakSpots);
    std::vector<Int_t>    GetPulseHeightHighRate(const std::vector<UShort_t> & trace,const std::vector<Int_t> &PeakSpots);

    double getFunc(TMatrixT<Double_t>, double);
    double getFunc(std::vector<double>&, double );
    Double_t DoMatrixInversionAlgorithm(const std::vector <Double_t> & CFD, Int_t Spot);
    std::vector <Double_t> GetMatrixInversionAlgorithmCoeffients(const std::vector <Double_t> & CFD, Int_t& Spot);
    Double_t GetZeroCubicRMD(std::vector <Double_t> &,int &);///<Extracts zero-crossing CFD (cubic-fit) with a different search algorithm, better suited for slow signals
    Double_t GetZeroCubic(std::vector <Double_t> &,Int_t &);
    Double_t GetZeroFitCubic(std::vector <Double_t> &);
    Double_t fitTrace(std::vector <UShort_t> &,Double_t, Double_t );
    Double_t GetEnergy(std::vector <UShort_t> &trace,Int_t MaxSpot);
    Double_t GetEnergyRMD(std::vector <UShort_t> &trace,Int_t MaxSpot);///<Integrates trace around maximum using asymmetric limits (better suited for slow signals with long decay tails)
    std::vector<Double_t> GetEnergyHighRate(const std::vector <UShort_t> & trace,std::vector<Int_t> &PeakSpots,std::vector<Double_t>& theUnderShoots,Double_t & MaxValueOut,Int_t & MaxIndexOut);
    Double_t GetGate(std::vector <UShort_t> &trace, int start, int L);
    Double_t GetGateRMD(std::vector <UShort_t> &trace, int &start, int L);///<Integrates trace from "raising" point (calculated from trace derivative) over range L
    Int_t GetMaxPulseHeight(std::vector <UShort_t> &trace, Int_t &MaxSpot);
    Int_t GetMaxPulseHeight(std::vector <Double_t> &trace, Int_t &MaxSpot);
    Double_t GetLeadingEdge(std::vector <UShort_t> &trace);///<Calculates Leading-edge time
    Double_t GetFilterEnergy(std::vector <UShort_t> &trace);///<Calculates energy from trace-analysis using trapezoidal-filter (see. Tan et al., IEEE trans. of nucl. sci., 51, 1541 (2004) ) 
    Double_t GetMaxPulseHeightBaseline(std::vector <UShort_t> &trace, Int_t &MaxSpot);
    Double_t GetPulseComplete(std::vector <UShort_t> &trace,Int_t &MaxSpot,Double_t &rising,Double_t &background,Double_t &noise);///<Takes trace and calculates pulse-hight (minus baseline), rising time, baseline, and noise level
    Int_t GetStartForPulseShape(Int_t MaxSpot);

    Int_t CalculateCFD(std::vector<UShort_t> trace, std::vector <Double_t> & );
    std::vector <Double_t> GetNewFirmwareCFD(const std::vector<UShort_t> & trace, Int_t FL, Int_t FG, Int_t d, Int_t w);
    Double_t GetNewFirmwareCFDWeight(Int_t CFDScaleFactor);

    TVector3 GetPosition() { return TVector3(0,0,1); }

    Double_t numOfBadFits;

  private:
  ClassDef(TLendaFilter,1)
};

#endif
