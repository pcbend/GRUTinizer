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

  //////////////////////////////////////////////
  virtual void InsertHit(const TDetectorHit&);
  virtual TDetectorHit& GetHit(int i);
  /////////////////////////////////////////////

  void SetEventCounter(Long_t event) { fEventCounter = event; }

  Long_t GetEventCounter() { return fEventCounter;}
  Long_t GetTimestamp()    { return Timestamp(); }

  TVector3 CRDCTrack();  // not a finished method
  TVector3 ExitTargetVect(int order=6);
  Float_t Azita(int order=6);

  //TVector3 Track() const;  //  s800 track with respect to optical axis
  TVector3 Track(double ata=0.000,double bta=0.000) const; // s800 track with respect to vector with ata,bta.

  float GetXFP(int i=0) const; // x position in the first(second) CRDC (mm)
  float GetYFP(int i=0) const; // y position in the first(second) CRDC (mm)
  float GetAFP() const; // x-angle in the focal plane (rad)
  float GetBFP() const; // y-angle in the focal plane (rad)

  Float_t GetAta(int i=6) const; // x-angle at the target (rad)
  Float_t GetYta(int i=6) const; // y-offset at the target (mm)
  Float_t GetBta(int i=6) const; // y-angle at the target (rad)
  Float_t GetDta(int i=6) const; // dE/E of outgoing particle, relative to the central b-rho

  float AdjustedBeta(float) const;

  virtual void Copy(TObject& obj)        const;
  virtual void Print(Option_t *opt = "") const {;}
  virtual void Clear(Option_t* opt = "");

  TCrdc         &GetCrdc(int x=0)  const { if(x==0) return (TCrdc&)crdc1;
                                           else return (TCrdc&)crdc2;}
  TTof          &GetTof()          const { return (TTof&)tof;        }
  TMTof         &GetMTof()         const { return (TMTof&)mtof;        }
  TIonChamber   &GetIonChamber()   const { return (TIonChamber&)ion; }
  TScintillator &GetScint(int x=0) const { return (TScintillator&)scint[x]; }
  //Added 1/4/2016 for getting trigbit - BAE
  TTrigger      &GetTrigger()      const { return (TTrigger&)trigger;}

  
  float GetIonSum() const { return ion.GetSum(); }

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
  float GetOBJ_E1Raw_MESY_Ch15(int i=0) const;

  float GetXF_E1Raw_MESY(int i=0) const;
  float GetXF_E1Raw_MESY_Ch15(int i=0) const;
 
  float GetRawOBJ_MESY(unsigned int i=0) const;

  float GetRawE1_MESY(unsigned int i=0) const;
  float GetRawE1_MESY_Ch15(unsigned int i=0) const;
  
  float GetRawXF_MESY(unsigned int i=0) const;

  float GetMTOF_ObjE1(unsigned int i=0) const { return GetCorrTOF_OBJ_MESY(i); }
  float GetMTOF_XfpE1(unsigned int i=0) const { return GetXF_E1Raw_MESY(i);    }
  float GetMTOF_RfE1(unsigned int i=0)  const { return GetME1Up(i) - GetMRf(i);  }
  float GetMTOF_ObjRf(unsigned int i=0) const { return GetMRf(i)   - GetMObj(i); }


  float MCorrelatedOBJ() const;
  float MCorrelatedXFP() const;
  float MCorrelatedE1() const;
  float MCorrelatedOBJ_E1(bool corrected=true) const;
  float MCorrelatedXFP_E1(bool corrected=true) const;

  float MCorrelatedOBJ_Ch15() const;
  float MCorrelatedXFP_Ch15() const;
  float MCorrelatedE1_Ch15() const;
  float MCorrelatedOBJ_E1_Ch15(bool corrected=true) const;
  float MCorrelatedXFP_E1_Ch15(bool corrected=true) const;


  unsigned short GetME1Up(int i)       const {if(i<GetME1Size())   return mtof.fE1Up.at(i);  return sqrt(-1); }     
  unsigned short GetME1Down(int i)     const {if(i<GetME1Size())   return mtof.fE1Down.at(i);return sqrt(-1); }    
           int   GetME1Size()          const { return mtof.fE1Up.size();  }
  unsigned short GetMXfp(int i)        const {if(i<GetMXfpSize())    return mtof.fXfp.at(i);   return sqrt(-1); }
           int   GetMXfpSize()         const { return mtof.fXfp.size();  }
  unsigned short GetMObj(int i)        const {if(i<GetMObjSize())    return mtof.fObj.at(i);   return sqrt(-1); }
           int   GetMObjSize()         const { return mtof.fObj.size();  }
  unsigned short GetMRf(int i)         const {if(i<GetMRfSize())     return mtof.fRf.at(i);    return sqrt(-1);  }
           int   GetMRfSize()          const { return mtof.fRf.size();  }
  unsigned short GetMCrdc1Anode(int i) const {if(i<GetMCrdc1AnodeSize()) return mtof.fCrdc1Anode.at(i);return sqrt(-1); }
           int   GetMCrdc1AnodeSize()  const { return mtof.fCrdc1Anode.size();  }
  unsigned short GetMCrdc2Anode(int i) const {if(i<GetMCrdc2AnodeSize()) return mtof.fCrdc2Anode.at(i);return sqrt(-1); }
           int   GetMCrdc2AnodeSize()  const { return mtof.fCrdc2Anode.size();  }
  unsigned short GetMHodoscope(int i)  const {if(i<GetMHodoscopeSize()) return mtof.fHodoscope.at(i);  return sqrt(-1);  }
           int   GetMHodoscopeSize()   const { return mtof.fHodoscope.size();  }
  unsigned short GetMRef(int i)        const {if(i<GetMRefSize())       return mtof.fRef.at(i);        return sqrt(-1);  }        
           int   GetMRefSize()         const { return mtof.fRef.size();  }

  
  int GetReg() const { return trigger.GetRegistr(); } 

private:
  virtual int  BuildHits(std::vector<TRawEvent>& raw_data);

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
  TCrdc        crdc1;
  TCrdc        crdc2;
  //THodoscope   hodo[32];
  //TMultiHitTof multi_tof;
  
  Long_t fEventCounter;
  static bool fGlobalReset; //!


  public:
    static void DrawPID(Option_t *gate="",Option_t *opt="",Long_t entries=kMaxLong,TChain *chain=0);
    static void DrawAFP(Option_t *gate="",Option_t *opt="",Long_t entries=kMaxLong,TChain *chain=0);
    static void DrawDispX(Option_t *gate="",Option_t *opt="",Long_t entries=kMaxLong,TChain *chain=0);
    static void DrawPID_Tune(Long_t entries=kMaxLong,TChain *chain=0);
    
    static void DrawPID_Mesy(Option_t *gate="",Option_t *opt="",Long_t entries=kMaxLong,int i=0,TChain *chain=0);
    static void DrawAFP_Mesy(Option_t *gate="",Option_t *opt="",Long_t entries=kMaxLong,int i=0,TChain *chain=0);
    static void DrawDispX_Mesy(Option_t *gate="",Option_t *opt="",Long_t entries=kMaxLong,int i=0,TChain *chain=0);
    static void DrawPID_Mesy_Tune(Long_t entries=kMaxLong,int i=0,TChain *chain=0);

  void SetGlobalReset(bool flag=true) { fGlobalReset=flag; }  


  ClassDef(TS800,3);
};

#endif
