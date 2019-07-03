
#include "TRuntimeObjects.h"
#include "TChannel.h"

#include "TFSU.h"
#include "GCutG.h"


GCutG *time_thing=0; //These are global declarations for gating on already gated data
GCutG *prompt=0;



void DanParticle(TRuntimeObjects &obj, const char *dirname) {

  TFSU *fsu = obj.GetDetector<TFSU>(); //Gets Data

  //---------------------------------------------------------------------------------

  if(!fsu) {
    return;   //if there isn't any data this returns a zero
  }
  
  //---------------------------------------------------------------------------------


  TFSUHit E = fsu->GetE();       //Gives dE and E, the particle detectors
  TFSUHit dE = fsu->GetDeltaE();

  fsu->CleanHits(20,9500,2000); //low E, high E, time diff (in 10's of nano seconds)

  obj.FillHistogram(dirname,"Multiplicity",20,0,20,fsu->Size()); //The number of gamma rays detected with each particle

  bool gamma_check = false; 

  for(unsigned int i=0; i<fsu->Size(); i++){ //This is looping over all of the entries in fsu and if it has a 370keV detection
      TFSUHit hit = fsu->GetFSUHit(i);
      if(hit.GetEnergy() >= 367 && hit.GetEnergy() <= 373) {
     // if(hit.GetEnergy() >= 1235 && hit.GetEnergy() <= 1240) {
       if(fsu->Size()>1){ //so we don't just get one signal
          gamma_check = true;
       }
      }
  }

  for(unsigned int i=0; i<fsu->Size(); i++) {  //This loops over all the Gamma-ray events associated with each particle
    TFSUHit hit = fsu->GetFSUHit(i); //Gets the Gamma data
    bool IsPrompt = false;
    if(prompt && prompt->IsInside(dE.Timestamp() - hit.Timestamp(),hit.GetEnergy())) {
      IsPrompt = true;
    }


    obj.FillHistogram(dirname, "Gamma_TdE",200,-100,100,(dE.Timestamp() - hit.Timestamp()),
        4000,0,8000, hit.GetEnergy()); //Histograms that give the dT against gammaray E

    obj.FillHistogram(dirname, "Gamma_TE",200,-100,100,(E.Timestamp() - hit.Timestamp()),
        4000,0,8000, hit.GetEnergy());

    if(IsPrompt) {
      obj.FillHistogram(dirname, "Gamma_TdE_prompt",200,-100,100,(dE.Timestamp() - hit.Timestamp()),
          4000,0,8000, hit.GetEnergy()); //Histograms that give the dT against gammaray E

      obj.FillHistogram(dirname, "Gamma_TE_prompt",200,-100,100,(E.Timestamp() - hit.Timestamp()),
          4000,0,8000, hit.GetEnergy());
  
      obj.FillHistogram(dirname, "Gamma_spectrum_prompt", 14000,0,14000, hit.GetEnergy());
 
      for (unsigned int j=i+1;j<fsu->Size();j++){
        TFSUHit hit2 = fsu->GetFSUHit(j);
      
        obj.FillHistogram(dirname,"matrix_sort",4000,0,4000,hit.GetEnergy(),4000,0,4000,hit2.GetEnergy());//This is for sorting purposes!
        obj.FillHistogram(dirname,"matrix_sort",4000,0,4000,hit2.GetEnergy(),4000,0,4000,hit.GetEnergy());

      }

      if(gamma_check == true) {

        obj.FillHistogram(dirname, "G_S_370", 14000, 0, 14000, hit.GetEnergy());

        // if(hit.GetEnergy() >= 124 && hit.GetEnergy() <= 135) { //I think this is an old attempt at gating again?
          obj.FillHistogram(dirname, "e_timing stuff",200,-100,100,(dE.Timestamp() - E.Timestamp()),
                                                  2000,0,4000, dE.GetEnergy());
        //}
     } 

   } else {
      obj.FillHistogram(dirname, "Gamma_TdE_notprompt",200,-100,100,(dE.Timestamp() - hit.Timestamp()),
          4000,0,8000, hit.GetEnergy()); //Histograms that give the dT against gammaray E

      obj.FillHistogram(dirname, "Gamma_TE_notprompt",200,-100,100,(E.Timestamp() - hit.Timestamp()),
          4000,0,8000, hit.GetEnergy());

    }

}

  //----------------------------------------------------------------------------------------

  if(time_thing && time_thing->IsInside((dE.Timestamp() - E.Timestamp()),dE.GetEnergy())) {
    obj.FillHistogram(dirname,"PID_junk",2000,0,4000,E.GetEnergy(),
        2000,0,4000,dE.GetEnergy()); //PID

  }



  obj.FillHistogram(dirname, "ParticleT_dE",200,-100,100,(dE.Timestamp() - E.Timestamp()),
      2000,0,4000, dE.GetEnergy()); //Particle detcetion time diff against particle energy (dE and E)

  obj.FillHistogram(dirname, "ParticleT_E",200,-100,100,(dE.Timestamp() - E.Timestamp()),
      2000,0,4000, E.GetEnergy());

  obj.FillHistogram(dirname,"PID",2000,0,4000,E.GetEnergy(),
      2000,0,4000,dE.GetEnergy()); //PID


  if(fsu->Size() == 0) {    //Only consider events that have no associated gamma ray detections

    obj.FillHistogram(dirname, "ParticleT_dE_noGR",200,-100,100,(dE.Timestamp() - E.Timestamp()),
        2000,0,4000, dE.GetEnergy());

    obj.FillHistogram(dirname, "ParticleT_E_noGR",200,-100,100,(dE.Timestamp() - E.Timestamp()),
        2000,0,4000, E.GetEnergy());

    obj.FillHistogram(dirname,"PID_noGR",2000,0,4000,E.GetEnergy(),
        2000,0,4000,dE.GetEnergy());
  }
  else{    //Only consider events that have associated gamma ray detections

    obj.FillHistogram(dirname, "ParticleT_dE_withGR",200,-100,100,(dE.Timestamp() - E.Timestamp()),
        2000,0,4000, dE.GetEnergy());

    obj.FillHistogram(dirname, "ParticleT_E_withGR",200,-100,100,(dE.Timestamp() - E.Timestamp()),
        2000,0,4000, E.GetEnergy());

    obj.FillHistogram(dirname,"PID_withGR",2000,0,4000,E.GetEnergy(),
        2000,0,4000,dE.GetEnergy());


  }

}




bool once=false;

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {    // I get called for every row!!

  TFSU  *fsu = obj.GetDetector<TFSU>(); //Get data and gates 
  TList *gates = &(obj.GetGates());


  if(!fsu) {
    return;   //if there isn't any data this returns a zero
  } 

  TFSUHit E = fsu->GetE(); //Declare E and dE
  TFSUHit dE = fsu->GetDeltaE();

  //----------------------------------------------------------------
  bool zero_cut = true;

  for(int i=0;i<gates->GetEntries();i++) { //Applies DanParticle for every gate in linked file        
    GCutG* cut = (GCutG*)gates->At(i);
    char* str_add;
    //if(cut->GetTag()=="pid") {
    if(!once) 
      std::cout << "cut:\t" << cut->GetName() <<"\t" << cut->GetTag() << std::endl;

    if(!time_thing && strcmp(cut->GetTag(),"time")==0) {
      time_thing = cut;
      std::cout << "time gate loaded.\n" << std::endl;

    } else if(!prompt && strcmp(cut->GetName(),"TimeRandomCut")==0){
      prompt = cut;
      std::cout << "prompt gate loaded.\n" << std::endl;

    } else if(cut->IsInside(E.GetEnergy(),dE.GetEnergy())) {
      DanParticle(obj,cut->GetName());
      zero_cut = false;

    } else { 
      str_add = "No";
      strcat(str_add, cut->GetName());
      DanParticle(obj,str_add);
      {
      }
      once =true;

      if(zero_cut) {
	DanParticle(obj, "Zero_Cut");
      }


      //---------------------------------------------------------------------------------------------------

      for(unsigned int i=0;i<fsu->Size();i++) {  // Loops over the instances (GR detections)
	TFSUHit hit = fsu->GetFSUHit(i); //Gets GR data

	TFSUHit E  = fsu->GetE(); //Gets E and dE data
	TFSUHit dE = fsu->GetDeltaE();


	obj.FillHistogram("PID",2000,0,4000,E.GetEnergy(), 2000,0,4000,dE.GetEnergy()); //PID


	if(E.GetEnergy() < 0 && dE.GetEnergy() > 150){ 
	  obj.FillHistogram("dEwithoutE", 2000,0,4000, dE.GetEnergy()); //Histo of dE detections without E        
	}


	obj.FillHistogram("Time_dE_E",200,-100,100,(dE.Timestamp() - E.Timestamp()));  //Time difference between detections
	obj.FillHistogram("Time_dE_GR",200,-100,100,(dE.Timestamp() - hit.Timestamp()));


	obj.FillHistogram("Energy_E", 2000,0,4000, E.GetEnergy()); //Energy for E, dE, and GR
	obj.FillHistogram("Energy_dE", 2000,0,4000, dE.GetEnergy());
	obj.FillHistogram("Energy_GR", 12000,0,12000, hit.GetEnergy());


      }
  
    }
  }
}
