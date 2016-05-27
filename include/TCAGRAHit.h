#ifndef TCAGRAHIT_H
#define TCAGRAHIT_H

#include "TDetectorHit.h"
#include "TCAGRASegmentHit.h"

class TCAGRAHit : public TDetectorHit {
  public:
    TCAGRAHit();
    ~TCAGRAHit();

    virtual void Copy(TObject& obj) const;
    virtual void Print(Option_t *opt = "") const;
    virtual void Clear(Option_t *opt = "");
    //virtual void Draw(Option_t* opt = "");

    virtual Int_t Charge() const;

    int GetDetnum() const;
    int GetMainSegnum() const;

    bool HasCore() const;

    unsigned int GetNumSegments() const { return fSegments.size(); }
    TCAGRASegmentHit& GetSegment(int i) { return fSegments.at(i); }
    unsigned long GetSegmentTimestamp() {
        if(fSegments.size()){
            return fSegments[0].Timestamp();
        } else {
            return -1;
        }
    }

    TCAGRASegmentHit& MakeSegmentByAddress(unsigned int address);

    int GetBoardID() const;
    int GetChannel() const;

    TVector3 GetPosition(bool apply_array_offset = false) const;

    double GetDoppler(double beta,
                      const TVector3& particle_vec = TVector3(0,0,1),
                      const TVector3& cagra_offset = TVector3(0,0,0)) const;


  private:
    std::vector<TCAGRASegmentHit> fSegments;


  ClassDef(TCAGRAHit,1);
};


#endif
