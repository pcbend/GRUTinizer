#ifndef _TOBJ_H_
#define _TOBJ_H_

#include <TVector3.h>

#include <TDetector.h>
#include <TOBJHit.h>

#include <TChannel.h>
#include <GValue.h>

class TOBJ : public TDetector {
  public:
    TOBJ() { }
    ~TOBJ() { }

    void Copy(TObject &obj) const { } //TDetector::Copy(obj); }

    void Clear(Option_t *opt="") { TDetector::Clear(opt); obj_hits.clear(); }
    
    void InsertHit(const TDetectorHit&) {   }
    TDetectorHit &GetHit(int i) { return obj_hits.at(i); } 
    static TVector3 GetPosition(int i) { return TVector3(0,0,1); }

    size_t Size() const      { return obj_hits.size(); }


    TOBJHit GetOBJHit(int i) { return obj_hits[i]; } //.at(i);  }
 //   TOBJHit GetObj()    const { return fobj; }

    bool Good() const { return true; }


  //private:
    int BuildHits(std::vector<TRawEvent>& raw_data);
    //{ printf("lenda build hits called\n"); fflush(stdout);return 0;}

    
    std::vector<TOBJHit> obj_hits; 
//    TOBJHit fobj;

    
   ////////////////////
   ////////////////////
   ////////////////////
  ClassDef(TOBJ,4);
};

#endif
