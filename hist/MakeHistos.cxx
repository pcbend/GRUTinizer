#include "TRuntimeObjects.h"

#include <iostream>
#include <map>
// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
//
// Note: from TRuntimeObjects:
//    TList& GetDetectors();
//    TList& GetObjects();
//

#include <cstdio>

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include "TRandom.h"

#include "TObject.h"
#include "TFastScint.h"

TH1 *GetHistogram(TList *list, std::string histname,int xbins,double xlow,double xhigh) {
  //TList *list = &(obj.GetObjects());
  TH1   *hist = (TH1*)list->FindObject(histname.c_str());
  if(!hist) {
    hist= new TH1I(histname.c_str(),histname.c_str(),xbins,xlow,xhigh);
    list->Add(hist);
  }
  return hist;
}

TH2 *GetMatrix(TList *list, std::string histname,int xbins, double xlow,double xhigh,
                                                 int ybins, double ylow,double yhigh) {
  //TList *list = &(obj.GetObjects());
  TH2   *mat  = (TH2*)list->FindObject(histname.c_str());
  if(!mat) {
    mat = new TH2I(histname.c_str(),histname.c_str(),xbins,xlow,xhigh,
                                                     ybins,ylow,yhigh);
    list->Add(mat);
  }
  return mat;
}


extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  TFastScint *fast    = 0;
  TIter iter(&obj.GetDetectors());
  while(TObject *object = iter.Next()) {
    if(object->InheritsFrom(TFastScint::Class()))
        fast = (TFastScint*)object;
  }

  if(!fast)
    return;

  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();
 
  for(int i=0;i<fast->Size();i++) {
    TFastScintHit *hit = fast->GetLaBrHit(i);
    TH2 *chan_eng = GetMatrix(list,"ChannelCharge",8000,0,64000,20,0,20);
    chan_eng->Fill(hit->Charge(),hit->GetChannel());
  }
  if(numobj!=list->GetSize())
    list->Sort();
}
