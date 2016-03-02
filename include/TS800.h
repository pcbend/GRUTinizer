#ifndef _TS800HUNDRED_H_
#define _TS800HUNDRED_H_

#include "TClonesArray.h"
#include <iostream>
#include <vector>
#include "TDetector.h"

#include "GValue.h"
#include "TS800Hit.h"
//class TTrigger;

class TChain;

class TS800 : public TDetector {
public:
  TS800();
  virtual ~TS800();

  static void ReadInverseMap(const char *);

  //////////////////////////////////////////////
  virtual void InsertHit(const TDetectorHit&);
  virtual TDetectorHit& GetHit(int i);
  /////////////////////////////////////////////

  void SetEventCounter(Long_t event) { fEventCounter = event; }

  Long_t GetEventCounter() { return fEventCounter;}
  Long_t GetTimestamp()    { return Timestamp(); }

  TVector3 CRDCTrack();  // not a finished method
  TVector3 ExitTargetVect_Spec(int order=6);
  float GetAFP() const;
  float GetBFP() const;
  float MapCalc_SpecTCL(int calcorder,int parameter,float *input);

  Float_t GetAta_Spec(int i=6);
  Float_t GetYta_Spec(int i=6);
  Float_t GetBta_Spec(int i=6);
  Float_t GetDta_Spec(int i=6);

  //bool InvMapLoaded()      { return fMapLoaded; }

  virtual void Copy(TObject& obj)        const;
  //virtual void Print(Option_t *opt = "") const;
  virtual void Clear(Option_t* opt = "");

  TCrdc         &GetCrdc(int x=0)  const { return (TCrdc&)crdc[x];   }
  TTof          &GetTof()          const { return (TTof&)tof;        }
  TMTof         &GetMTof()         const { return (TMTof&)mtof;        }
  TIonChamber   &GetIonChamber()   const { return (TIonChamber&)ion; }
  TScintillator &GetScint(int x=0) const { return (TScintillator&)scint[x]; }
  //Added 1/4/2016 for getting trigbit - BAE
  TTrigger      &GetTrigger()      const { return (TTrigger&)trigger;}

  //float GetAFP() const {
  //    return  TMath::ATan((GetCrdc(0).GetDispersiveX()-GetCrdc(1).GetDispersiveX())/1000.0);
  //}

  //Note c1 is the AFP correction and c2 is the XFP correction
  float GetTofE1_TAC(float c1=0.00,float c2=0.00)  const;
  float GetTofE1_TDC(float c1=0.00,float c2=0.00)  const;
  float GetTofE1_MTDC(float c1=0.00,float c2=0.00,int i=0) const;

  float GetCorrTOF_OBJTAC() const;
  float GetOBJRaw_TAC() const;
  float GetXFRaw_TAC() const;
  
  float GetCorrTOF_OBJ() const;
  float GetOBJ_E1Raw() const;
  float GetXF_E1Raw() const;
  //===================================================  






  float GetCorrTOF_OBJ_MESY(int i=0) const;
  
  float GetOBJ_E1Raw_MESY(int i=0) const;
 
  float GetRawOBJ_MESY(int i=0) const;

  float GetRawE1_MESY(int i=0) const;
  
  float GetXF_E1Raw_MESY(int i=0) const;
  
  float GetRawXF_MESY(int i=0) const;

  float MCorrelatedOBJ() const;
  float MCorrelatedXFP() const;
  float MCorrelatedE1() const;
  float MCorrelatedOBJ_E1(bool corrected=true) const;
  float MCorrelatedXFP_E1() const;
  

private:

  //---------------------
  static std::vector<short> fmaxcoefficient;                      //!
  static std::vector<std::vector<short> > forder;                 //!
  static std::vector<std::vector<std::vector<short> > > fexponent;//!
  static std::vector<std::vector<float> > fcoefficient;           //!
  static short fmaxorder;                                         //!
  static float fbrho;                                             //!
  static int fmass;                                               //!
  static int fcharge;                                             //!
  //---------------------

  static bool ReadMap_SpecTCL(std::string);
  virtual int  BuildHits();

  bool HandleTrigPacket(unsigned short*,int);     //!
  bool HandleTOFPacket(unsigned short*,int);      //!
  bool HandleScintPacket(unsigned short*,int);    //!
  bool HandleIonCPacket(unsigned short*,int);     //!
  bool HandleCRDCPacket(unsigned short*,int);     //!
  bool HandleMTDCPacket(unsigned short*,int);     //!

  TScintillator scint[3];
  TTrigger     trigger;
  TTof         tof;
  TMTof        mtof;
  TIonChamber  ion;
  TCrdc        crdc[2];
  //THodoscope   hodo[32];
  //TMultiHitTof multi_tof;
  
  //std::map<int,Float_t> fAta;
  //std::map<int,Float_t> fYta;
  //std::map<int,Float_t> fBta;
  //std::map<int,Float_t> fDta;
  
  Float_t fAtaTCL1;
  Float_t fYtaTCL1;
  Float_t fBtaTCL1;
  Float_t fDtaTCL1;

  Float_t fAtaTCL2;
  Float_t fYtaTCL2;
  Float_t fBtaTCL2;
  Float_t fDtaTCL2;

  Float_t fAtaTCL3;
  Float_t fYtaTCL3;
  Float_t fBtaTCL3;
  Float_t fDtaTCL3;

  Float_t fAtaTCL4;
  Float_t fYtaTCL4;
  Float_t fBtaTCL4;
  Float_t fDtaTCL4;

  Float_t fAtaTCL5;
  Float_t fYtaTCL5;
  Float_t fBtaTCL5;
  Float_t fDtaTCL5;

  Float_t fAtaTCL6;
  Float_t fYtaTCL6;
  Float_t fBtaTCL6;
  Float_t fDtaTCL6;
  

  Long_t fEventCounter;


  public:
    static void DrawPID(Option_t *gate="",Option_t *opt="",Long_t entries=kMaxLong,TChain *chain=0);
    static void DrawAFP(Option_t *gate="",Option_t *opt="",Long_t entries=kMaxLong,TChain *chain=0);
    static void DrawDispX(Option_t *gate="",Option_t *opt="",Long_t entries=kMaxLong,TChain *chain=0);
    static void DrawPID_Tune(Long_t entries=kMaxLong,TChain *chain=0);
    
    static void DrawPID_Mesy(Option_t *gate="",Option_t *opt="",Long_t entries=kMaxLong,int i=0,TChain *chain=0);
    static void DrawAFP_Mesy(Option_t *gate="",Option_t *opt="",Long_t entries=kMaxLong,int i=0,TChain *chain=0);
    static void DrawDispX_Mesy(Option_t *gate="",Option_t *opt="",Long_t entries=kMaxLong,int i=0,TChain *chain=0);
    static void DrawPID_Mesy_Tune(Long_t entries=kMaxLong,int i=0,TChain *chain=0);

  ClassDef(TS800,1);
};

//S8OO PIN
//  -- e
//  -- t
//  -- ecorr
//  -- dE
//

//S800 PPAC
//  -- u
//  -- d
//  -- l
//  -- r
//  -- a
//  -- t
//  -- sumx
//  -- sumy
//  -- x
//  -- y
//  --
//  -- z
//





//S800 Focal Plane
//  -- FpScintillator E1
//  -- FpScintillator E2  <-- most likely does not exist
//  -- FpScintillator E3  <-- most likely does not exist
//  -- FpIonChamber
//  -- FpCRDC  crdc1
//  -- FpCRDC  crdc2
//  -- FpTrack track
//  -- FpHodo  hodo


//S800 Target
//  -- PPac ppac2 (why 2?) <-- class
//  -- PIN  pin1 <-- class
//  -- PIN  pin2 <-- class
//  -- E

//S800 Image
//  -- Classic PPAC ppac1
//  -- Classic PPAC ppac2
//  -- double x
//  -- double theta
//  -- double y
//  -- double phi
//  -- double dp
//  --        z
//  --        gap
//  --        dpphi
//  --
//  -- bool trigger
//  -- char *gate

//S800 Object
//  -- PIN   <-- class

//S800 Trigger
//  -- registr
//  -- s800
//  -- external1
//  -- external2
//  -- secondary
//  -- timestamp

//S800 Time Of Flight
//  -- rf
//  -- obj
//  -- xfp
//  -- xfp_obj
//  -- rfe1
//  -- obje1
//  -- xfpe1
//  -- obje2
//  -- xfpe2
//  -- tac_obj
//  -- tac_obje1
//  -- tac_xfp
//  -- tac_xfpe1
//  -- tac_xfpobj
//  -- tacobj_obj
//  -- tacxfp_xfp
//  --
//  -- mesyhit
//  -- me1up
//  -- me1dwn
//  -- mobj
//  -- mxfp
//  -- mrf
//  -- mcrdc1
//  -- mcrdc2
//  -- mhoth
//  -- mref
//  -- mobje1
//  -- mxfpe1
//  -- mrfe1
//  --
//  -- diaor
//  -- dia1
//  -- dia2
//  -- dia3
//  -- dia4
//  -- dia1RF
//  -- dia2RF
//  -- dia3RF
//  -- dia4RF
//  -- diaRF
//  -- dia1Cor
//  -- dia2Cor
//  -- dia3Cor
//  -- dia4Cor
//  -- diaCor
//  --
//  -- obj_shift
//  -- xfp_shift
//  -- rf_shift
//  -- tac_obj_shift
//  -- tac_xfp_shift
//  --
//  --              // sc800.h +1253
//


//S800 LaBr
//  -- eraw
//  -- ecal
//  -- traw
//  -- tcal
//  -- e_a0
//  -- e_a1
//  -- e_a2
//  -- t_a0
//
//
//




#endif
