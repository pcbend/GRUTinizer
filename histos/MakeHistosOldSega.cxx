#include "TObject.h"
#include "TS800.h"
#include "TOldSega.h"
#include "TRuntimeObjects.h"

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {


  TS800 *s800 = obj.GetDetector<TS800>();


  if(!s800){
    return;
  }
  
  double s800_time = s800->GetCorrTOF_OBJ(); 
  double ion_chamber_sum = s800->GetIonChamber().Charge();
  double crdcx1 = s800->GetCrdc(0).GetDispersiveX();
  double afp = s800->GetAFP();

  obj.FillHistogram("S800","time_vs_ic",
                     1000,-1000,1000,s800_time,
	             1000,0,15000,ion_chamber_sum);

  obj.FillHistogram("S800","time_vs_crdcx1",
                     1000,-1000,1000,s800_time,
	             350,-50,300,crdcx1);

  obj.FillHistogram("S800","time_vs_afp",
                     1000,-1000,1000,s800_time,
	             1000,-0.5,0.5,afp);
  

}
