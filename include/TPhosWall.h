#ifndef TPHOSWALL_H 
#define TPHOSWALL_H 

#include <TObject.h>
#include <TVector3.h>

#include <PWFragment.h>
#include <PWSegment.h>

#include <map>

#define MAXPIXEL 256

class TPhosWall : public TObject {
  
  public:
    TPhosWall();
    TPhosWall(TGEBEvent&);
    ~TPhosWall();

    void Copy(const TGEBEvent&);

    //void AddPWHit(PWFragment &frag);
    
    const int  Size()     { return fNumberOfHits; }

    const inline Int_t    GetAddress()  { if(Size()<1) return -1; return Address(GetLargestNumber()); }
    const inline Int_t    GetA()        { if(Size()<1) return -1; return A(GetLargestNumber()); }
    const inline Int_t    GetB()        { if(Size()<1) return -1; return B(GetLargestNumber()); }
    const inline Int_t    GetC()        { if(Size()<1) return -1; return C(GetLargestNumber()); }
                 Float_t  GetACal();    
                 Float_t  GetBCal();    
                 Float_t  GetCCal();    

    const inline Int_t    GetBDiff()    {if(Size()<1) return -1; if(GetLargestNumber()>Size()) return -1; return BDiffCal(GetLargestNumber());}

    const inline Int_t    GetTime()     { if(Size()<1) return -1;  return Time(GetLargestNumber()); }
          const  Int_t    GetLargestNumber() { return fLargestHit; }  
    const inline Int_t    GetPixel()    { if(Size()<1) return -1; if(GetLargestNumber()>Size()) return -1; return Pixel(GetLargestNumber()); }
    const inline TVector3 GetPosition() { return fWeightedPosition; }

    const inline Int_t    GetASum()   { int chg=0; for(int x=0;x<Size();x++) {chg+=fACharge[x];} return chg;}
    const inline Int_t    GetBSum()   { int chg=0; for(int x=0;x<Size();x++) {chg+=fBCharge[x];} return chg;}
    const inline Int_t    GetCSum()   { int chg=0; for(int x=0;x<Size();x++) {chg+=fCCharge[x];} return chg;}

    const inline Int_t    GetBDiffSum() { int chg=0; for(int x=0;x<Size();x++) {chg+=BDiffCal(x);} return chg;}

    const inline Int_t    Address(const int &i)  const {  return  fAddress[i]; }
    const inline Int_t    A(const int &i)        const {  return  (Int_t)fACharge[i]; }
    const inline Int_t    B(const int &i)        const {  return  (Int_t)fBCharge[i]; }
    const inline Int_t    C(const int &i)        const {  return  (Int_t)fCCharge[i]; }
               Float_t    ACal(const int &i);
               Float_t    BCal(const int &i);
               Float_t    CCal(const int &i);
                 Int_t    BDiffCal(const int &i);
    const inline Int_t    Time(const int &i)     const {  return  (Int_t)fTime[i];  }
    const inline Int_t    Pixel(const int &i)    const {  return  (Int_t)fPixel[i]; }
    const inline TVector3 *Position(const int &i)  {  return &fPosition[i];     }

    Int_t    GetASmartSum(float res = 20.0,int threshold=200); 
    Int_t    GetBSmartSum(float res = 20.0,int threshold=200);
    Int_t    GetCSmartSum(float res = 20.0,int threshold=200); 
    //inline const int NumberOfPWHits() { return PhosWallHits.size(); }

    Float_t  GetACalSmartSum(float res = 20.0,Float_t threshold=0.10); 
    Float_t  GetBCalSmartSum(float res = 20.0,Float_t threshold=0.10);
    Float_t  GetCCalSmartSum(float res = 20.0,Float_t threshold=0.10); 



    void Print(Option_t *opt = "");
    void Clear(Option_t *opt = "");
    void Draw(Option_t *opt="");
    void DrawXY(Option_t *opt="");

    static TVector3 *FindWallPosition(const Int_t &pixel) { if(pixel<0||pixel>256) return beam;  return fWallPositions[pixel]; }
    void FindWeightedPosition();
    void SetWeightedPosition(const TVector3 &temp)  { fWeightedPosition = temp; }    

    TVector3 GetWallPosition(int pixelnumber,double delta=5.0);

    Int_t Multiplicity() { return fNumberOfHits; }

    inline const Long_t GetTimeStamp() { return fTimeStamp; }

  private:
    //std::vector<PWFragment> PhosWallHits;
    static TVector3 *beam; //!
    static int fHitPatternCounter;


    Long_t   fTimeStamp;
    Short_t  fLargestHit;
    TVector3 fWeightedPosition;

    Int_t fNumberOfHits;


    TVector3 *fPosition;  //[fNumberOfHits]
    Int_t    *fAddress;   //[fNumberOfHits]
    Short_t  *fACharge;   //[fNumberOfHits]
    Short_t  *fBCharge;   //[fNumberOfHits]
    Short_t  *fCCharge;   //[fNumberOfHits]
    Short_t  *fPixel;     //[fNumberOfHits]
    Short_t  *fTime;      //[fNumberOfHits]
    //std::vector<TVector3> fPosition;
    //std::vector<Int_t>    fAddress;
    //std::vector<Short_t>  fACharge;
    //std::vector<Short_t>  fBCharge;
    //std::vector<Short_t>  fCCharge;
    //std::vector<Short_t>  fPixel;
    //std::vector<Short_t>  fTime;


    static TVector3 *fWallPositions[257];
    static bool      fPositionsSet;
    static void      SetWallPositions();


    void SetCalMaps();
    static bool fCalMapsSet;
    static std::map<int,std::pair<float,float> > fAMap;
    static std::map<int,std::pair<float,float> > fBMap;
    static std::map<int,std::pair<float,float> > fCMap;

    static std::map<int,int> fBDiffMap;

  ClassDef(TPhosWall,2);
};

#endif


