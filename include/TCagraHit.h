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

    void SetDiscTime(const ULong_t t) { time = t; }
    ULong_t GetDiscTime() { return time; }
    double GetBLCorrectedE() const;


  private:
    std::vector<TCagraSegmentHit> fSegments;
    ULong_t time;

  ClassDef(TCagraHit,1);
};


#endif
