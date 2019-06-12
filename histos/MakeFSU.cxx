
#include "TRuntimeObjects.h"
#include "TChannel.h"

#include "TFSU.h"
#include "GCutG.h"

void HandleFSU(TRuntimeObjects &obj,const char *dirname) {
    TFSU  *fsu = obj.GetDetector<TFSU>();
    
    if(!fsu) {
      return;
    } 
    fsu->CleanHits(10.0,9000.0,40.0);
    if(!fsu->Size()) {
      return;
    }    

    TFSUHit E = fsu->GetE();
    TFSUHit dE = fsu->GetDeltaE();


    obj.FillHistogram(dirname,"Multiplicity", 20,0,20,fsu->Size());


    for(unsigned int i=0;i<fsu->Size();i++) {
      TFSUHit hit = fsu->GetFSUHit(i);
      
      if(hit.GetEnergy()>9000) continue;
      if(hit.GetEnergy()<10)   continue;

      obj.FillHistogram(dirname,"energy",8000,0,4000,hit.GetEnergy());
      obj.FillHistogram(dirname,"summary",8000,0,4000,hit.GetEnergy(),
                                            100,0,100,hit.GetNumber());
      obj.FillHistogram(dirname, "GammaDeTime",2000,-1000,1000, dE.Timestamp()-hit.Timestamp(),
                                               2000,0,4000,hit.GetEnergy());

      obj.FillHistogram("dE / dt 2D",200,-100,100,(dE.Timestamp() - hit.Timestamp()),
                                     2000,0,4000, dE.GetEnergy());
      obj.FillHistogram("E / dt 2D",200,-100,100,(dE.Timestamp() - hit.Timestamp()),
                                    2000,0,4000, E.GetEnergy());

       
      for(unsigned int j=i+1;j<fsu->Size();j++){
      TFSUHit hit2 = fsu->GetFSUHit(j);
      
      obj.FillHistogram(dirname,"matrix",4000,0,4000,hit.GetEnergy(),4000,0,4000,hit2.GetEnergy());
      obj.FillHistogram(dirname,"matrix",4000,0,4000,hit2.GetEnergy(),4000,0,4000,hit.GetEnergy());

      }

    }
}

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
    TFSU  *fsu = obj.GetDetector<TFSU>();
    //  TList *list  = &(obj.GetObjects());
    TList *gates = &(obj.GetGates());
    //  int numobj = list->GetSize()
    //  CleanHits(double low,double high,double timediff) {



    if(!fsu) {
      return;
    } 
   
//-----------------------------------------------------------------------------------------------------------------Dan TEST

//  TFSUHit E = fsu->GetE();
//  TFSUHit dE = fsu->GetDeltaE();

//  for(unsigned int i=0;i<fsu->Size();i++) {
//    TFSUHit hit = fsu->GetFSUHit(i);

//    TFSUHit E_it  = fsu->GetE();
//    TFSUHit dE_it = fsu->GetDeltaE();

//    double EdEdT = E_it.Timestamp() - dE_it.Timestamp();

//    if (EdEdT < 1000){
//      obj.FillHistogram("E/dE when dT < 10 (units)",2000,0,4000,E_it.GetEnergy(),
//                                                    2000,0,4000,dE_it.GetEnergy());
//    }


//    obj.FillHistogram("Energy",2000,0,4000, hit.GetEnergy();

//  }

//-----------------------------------------------------------------------------------------------------------------Dan TEST

    TFSUHit E = fsu->GetE();
    TFSUHit dE = fsu->GetDeltaE();

    obj.FillHistogram("EdE", 2000, 0, 4000, E.GetEnergy(),
            2000, 0, 4000, dE.GetEnergy());
    
    fsu->CleanHits(10.0,9000.0,40.0);
    if(!fsu->Size()) {
      return;
    }  



    for(int i=0;i<gates->GetEntries();i++) {
      GCutG* cut = (GCutG*)gates->At(i);
        //if(cut->GetTag()=="pid") {
            if(cut->IsInside(E.GetEnergy(),dE.GetEnergy())) {
                HandleFSU(obj,cut->GetName());
            }
        //}
    }


    for(unsigned int i=0;i<fsu->Size();i++) {  
        TFSUHit hit = fsu->GetFSUHit(i);
        obj.FillHistogram("energy",8000,0,4000,hit.GetEnergy());
        obj.FillHistogram("summary",8000,0,10000,hit.GetEnergy(),
                                     50,0,50,hit.GetNumber());


        obj.FillHistogram("GammaDeTime",2000,-1000,1000, dE.Timestamp()-hit.Timestamp(),
                2000,0,4000,hit.GetEnergy());




        for(unsigned int j=i+1;j<fsu->Size();j++) {  

            TFSUHit hit2 = fsu->GetFSUHit(j);

            if(hit.Timestamp()>hit2.Timestamp()){
 
    //  if(numobj!=list->GetSize())
    //    list->Sort();
                obj.FillHistogram("detime",2000,-1000,1000,hit.Timestamp()-hit2.Timestamp(), 
                        2000,0,4000, hit2.GetEnergy());
            }
            else{
                obj.FillHistogram("detime",2000,-1000,1000,hit2.Timestamp()-hit.Timestamp(), 
                        2000,0,4000, hit.GetEnergy());
            }

            obj.FillHistogram("dtime",4000,-2000,2000,hit.Timestamp()-hit2.Timestamp());
            obj.FillHistogram("matrix",4000,0,4000,hit.GetEnergy(),
                    4000,0,4000,hit2.GetEnergy());
            obj.FillHistogram("matrix",4000,0,4000,hit2.GetEnergy(),
                    4000,0,4000,hit.GetEnergy());


        }
        }

    }

    //  if(numobj!=list->GetSize())
    //    list->Sort();
                
