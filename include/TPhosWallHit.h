#ifndef _TPHOSWALLHIT_H_
#define _TPHOSWALLHIT_H_


class TPhosWallHit : public TObject {
  public:
    TPhosWall();
    virtual ~TPhosWall();

    virtual void Copy(TObject &obj) const;
    virtual void Print(Option_t *opt) const;
    virtual void Clear(Option_t *opt);


  private:
    Short_t  fACharge;   
    Short_t  fBCharge;   
    Short_t  fCCharge;   
    Short_t  fPixel;     
    Short_t  fTime;      
    
    TVector3 fPosition;  
    UInt_t    fAddress;   

  ClassDef(TPhosWallHit,1)

};

#endif

