#ifndef _TFASTSCINTDETECTORHIT_H_
#define _TFASTSCINTDETECTORHIT_H_

#include "TDetectorHit.h"

class TFastScintHit : public TDetectorHit {
  public:
    TFastScintHit();
    TFastScintHit(const TDetectorHit&);

    virtual void Print(Option_t* opt = "") const;
    virtual void Clear(Option_t* opt = "");
    virtual void Copy(TObject& obj) const;

    int  GetChannel()    const { return Address() & 0x1f;   }

    TVector3 &GetPosition() const;

    void SetTime(unsigned short time) { fTimes.push_back(time); }
    virtual int  Time() const { 
      if(fTimes.size()) {
	return fTimes[0]; 
      } else {
	return -1;
      }
    }

    const std::vector<unsigned short>& GetAllTimes() const { return fTimes; }
    size_t GetNumTimes() const { return fTimes.size(); }
    int GetMTime(size_t j) const { 
      //if(GetNumTimes()<j) {
	return fTimes[j]; 
      //} else {
	//return -1;
      //}
    }


    bool operator<(const TFastScintHit &rhs) const { return Charge()>rhs.Charge(); } //  fAddress<rhs.fAddress; }

  private:

    std::vector<unsigned short> fTimes;

  ClassDef(TFastScintHit,24)
};

#endif /* _TFASTSCINTDETECTORHIT_H_ */
