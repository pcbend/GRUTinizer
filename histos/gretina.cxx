#include "TRuntimeObjects.h"
#include "TGretina.h"


void MakeGretina(TRuntimeObjects& obj, TGretina& gretina) {

  std::string dirname = "Gretina";

  for(unsigned int i=0;i<gretina.Size();i++) {

    TGretinaHit gretina_hit = gretina.GetGretinaHit(i);
    if(gretina_hit.GetDetnum() == -1) continue;

    obj.FillHistogram(dirname,"CoreEnergy",4096,0,4096,gretina_hit.GetCoreEnergy());
    obj.FillHistogram(dirname,"CoreVdet",48, 0, 48, gretina_hit.GetArrayNumber(), 4096, 0, 4096, gretina_hit.GetCoreEnergy());
    obj.FillHistogram(dirname,Form("Core%i",gretina_hit.GetArrayNumber()), 4096, 0, 4096, gretina_hit.GetCoreEnergy());
  }//end hit loop

  return;
}

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {

  TGretina* gretina = obj.GetDetector<TGretina>();

  if(gretina) {
    MakeGretina(obj,*gretina);
  }

  return;
}

