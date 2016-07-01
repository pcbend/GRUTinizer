#ifndef TCagraHIT_H
#define TCagraHIT_H

#include "TDetectorHit.h"
#include "TCagraSegmentHit.h"

class TCagraHit : public TDetectorHit {
  public:
    TCagraHit();
    ~TCagraHit();

    virtual void Copy(TObject& obj) const;
    virtual void Print(Option_t *opt = "") const;
    virtual void Clear(Option_t *opt = "");
    //virtual void Draw(Option_t* opt = "");

    virtual Int_t Charge() const;

    int GetDetnum() const;
    char GetLeaf() const;
    int GetMainSegnum() const;

    bool HasCore() const;

    unsigned int GetNumSegments() const { return fSegments.size(); }
    TCagraSegmentHit& GetSegment(int i) { return fSegments.at(i); }
    unsigned long GetSegmentTimestamp() {
        if(fSegments.size()){
            return fSegments[0].Timestamp();
        } else {
            return -1;
        }
    }

    TCagraSegmentHit& MakeSegmentByAddress(unsigned int address);

    int GetBoardID() const;
    int GetChannel() const;

    TVector3 GetPosition(bool apply_array_offset = false) const; // modified from true

    double GetDoppler(double beta,
                      const TVector3& particle_vec = TVector3(0,0,1),
                      const TVector3& cagra_offset = TVector3(0,0,0)) const;

    void SetDiscTime(const Double_t t) { time = t; }
    Double_t GetDiscTime() { return time; }
    Double_t GetCorrectedEnergy(Double_t asym_bl=0.);
    void SetPreRise(Double_t prerise) { prerise_energy = prerise; }
    void SetPostRise(Double_t postrise) { postrise_energy = postrise; }

    std::vector<Short_t>* GetTrace(int segnum=0);
    void SetTrace(std::vector<Short_t>& trace);
    void DrawTrace(int segnum);
    double GetTraceHeight() const;
    double GetTraceHeightDoppler(double beta,const TVector3& vec = TVector3(0,0,1)) const;
    Double_t GetTraceEnergy(const UShort_t& a,const UShort_t& b,const UShort_t& x,const UShort_t& y) const;
    Double_t GetTraceBaseline();

  private:
    std::vector<Short_t> fTrace;
    std::vector<TCagraSegmentHit> fSegments;
    Double_t time;
    Double_t prerise_energy;
    Double_t postrise_energy;
    //Double_t fPZEnergy;
    Double_t fCorEnergy;

  ClassDef(TCagraHit,1);
};


#endif
