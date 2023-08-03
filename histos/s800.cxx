#include "TRuntimeObjects.h"
#include "TS800.h"
#include "TGates.h"
#include "GValue.h"
void MakeS800(TRuntimeObjects& obj, TS800& s800) {
  std::string dirname = Form("S800");
/*  obj.FillHistogram(Form("S800"), Form("obj"), 6000, -6000, 12000, s800.GetMTof().GetCorrelatedObjE1());
  obj.FillHistogram(Form("S800"), Form("xfp"), 6000, -2000, 10000,  s800.GetMTof().GetCorrelatedXfpE1());

  obj.FillHistogram(Form("S800"), Form("objxfp"), 3000, -7000, -1000, s800.GetMTof().GetCorrelatedObjE1(), 3000, 0, 6000,  s800.GetMTof().GetCorrelatedXfpE1());

  obj.FillHistogram(Form("S800"), Form("objnew"), 6000, -6000, 12000, s800.GetMTof().GetCorrelatedTof(3, 0, -4340, 0));
  obj.FillHistogram(Form("S800"), Form("xfpnew"), 6000, -2000, 10000,  s800.GetMTof().GetCorrelatedTof(2, 0, 1645, 0));

  obj.FillHistogram(Form("S800"), Form("objxfpnew"), 3000, -7000, -1000, s800.GetMTof().GetCorrelatedTof(3, 0, -4340, 0), 3000, 0, 6000,  s800.GetMTof().GetCorrelatedTof(2, 0, 1645, 0));
*/
  obj.FillHistogram(Form("crdc"), Form("crdcx1"), 800, -400, 400, s800.GetCrdc(0).GetDispersiveX());
  obj.FillHistogram(Form("crdc"), Form("crdcx2"), 800, -400, 400, s800.GetCrdc(1).GetDispersiveX());

  obj.FillHistogram(Form("crdc"), Form("MaxPad1"), 300,0, 300, s800.GetCrdc(0).GetMaxPad());
  obj.FillHistogram(Form("crdc"), Form("MaxPad2"), 300,0, 300, s800.GetCrdc(1).GetMaxPad());

}
// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {

  TS800* s800 = obj.GetDetector<TS800>();
  if(s800) {
    MakeS800(obj,*s800);
  }

  return;
}

