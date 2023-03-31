#include "TRuntimeObjects.h"
#include "TS800.h"
#include "TGates.h"
#include "GValue.h"
void MakeS800(TRuntimeObjects& obj, TS800& s800, TGates& incoming) {
  std::string dirname = Form("S800");
  obj.FillHistogram(Form("S800"), Form("objVxfp"), 3000, -7000, -1000, s800.GetMTof().GetCorrelatedObjE1(), 3000, 0, 6000,  s800.GetMTof().GetCorrelatedXfpE1());
  int gateID = incoming.GateID(s800.GetMTof().GetCorrelatedObjE1(), s800.GetMTof().GetCorrelatedXfpE1());
  if(gateID != -1) {
    obj.FillHistogram(Form("S800"), Form("objvsICSUM"), 501, -4500, -3000, s800.GetMTofObjE1(),  1000, 0, 3500, s800.GetIonChamber().GetAve());
  }
}
// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {

  TS800* s800 = obj.GetDetector<TS800>();
  TGates *incoming_gates = new TGates();
  incoming_gates->LoadPIDFile("/mnt/analysis/pecan-2015/sgillespie/SortCode/SortCode/PIDGate/INCOMING.pid");
//  incoming_gates->LoadPIDFile(Form("%s",GValue::Value("INCOMING")));
  if(s800) {
    MakeS800(obj,*s800, *incoming_gates);
  }

  return;
}

