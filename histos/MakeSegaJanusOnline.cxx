#include "TRuntimeObjects.h"

#include <cstdio>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TObject.h"
#include "TRandom.h"

#include "GValue.h"
#include "TJanusDDAS.h"
#include "TNSCLScalers.h"
#include "TSega.h"
#include "GCutG.h"



int gates_loaded=0;
std::vector<GCutG*> kin_gates;

void MakeSega(TRuntimeObjects& obj) {
	TSega* sega = obj.GetDetector<TSega>();
	TJanusDDAS* janus = obj.GetDetector<TJanusDDAS>();
	for(size_t x=0;x<sega->Size();x++) {
		TSegaHit &hit = sega->GetSegaHit(x);
		obj.FillHistogram("sega_summary",16,0,16,hit.GetDetnum(),
				3000,0,3000,hit.GetEnergy());
		obj.FillHistogram("sega_uptime",540,0,5400,hit.Timestamp()/1e9,
				20,0,20,hit.GetDetnum());

		//obj.FillHistogram(Form("Sega%02i",hit.GetDetnum()),"energy/time",3600,0,7200,hit.Timestamp()/1e9,
		//                                                                 1000,0,4000,hit.GetEnergy());
		obj.FillHistogram("sega",Form("energy_time_%02i",hit.GetDetnum()),3600,0,7200,hit.Timestamp()/1e9,
				100,1400,1500,hit.GetEnergy());

		for(int y=0;y<hit.GetNumSegments();y++) {
			TSegaSegmentHit shit = hit.GetSegment(y);
			obj.FillHistogram("sega",Form("Segments%02i",hit.GetDetnum()),
					1000,0,15000,shit.Charge(),
					32,0,32,shit.GetSegnum()-1);
		}

	}
	return; 
}

void MakeRawJanus(TRuntimeObjects& obj) {
	TJanusDDAS* janus = obj.GetDetector<TJanusDDAS>();
	std::vector<double> charge;
	for(size_t x=0; x<janus->GetAllChannels().size();x++) {
		TJanusDDASHit hit = janus->GetJanusChannel(x);
		obj.FillHistogram("multi",100,0,100,janus->GetAllChannels().size(),
				1500,0,15000,hit.Charge());
		obj.FillHistogram("janus_uptime",540,0,5400,hit.Timestamp()/1e9,
				200,0,200,hit.GetFrontChannel());


		obj.FillHistogram("janus_summary",200,0,200,hit.GetFrontChannel(),
				1500,0,15000,hit.Charge());


		if(hit.Charge()>150)
			charge.push_back(hit.Charge());
	}

	for(size_t x=0;x<charge.size();x++) {
		obj.FillHistogram("multi_150gated",20,0,20,charge.size(),
				1500,0,15000,charge.at(x));
	}

	return;
}

void MakeCalJanus(TRuntimeObjects &obj) {
	TJanusDDAS* janus = obj.GetDetector<TJanusDDAS>();
	TSega* sega = obj.GetDetector<TSega>();
	for(int x=0;x<janus->Size();x++) {
		TJanusDDASHit jhit = janus->GetJanusHit(x);
		obj.FillHistogram("janus_kin",90,0,180,jhit.GetPosition(0).Theta()*TMath::RadToDeg(),
				1800,0,18000,jhit.GetEnergy());
		if(sega)  {                                     
			for(int y=0;y<sega->Size();y++) {
				TSegaHit shit = sega->GetSegaHit(y);
				obj.FillHistogram("doppler_plus_any",1000,0,3000,shit.GetDoppler(0.08,jhit.GetPosition()));
				obj.FillHistogram("sega_plus_any",1500,0,3000,shit.GetEnergy());
				for(int z=0;z<kin_gates.size();z++) {
					GCutG *cut = kin_gates.at(z);
					if(!cut->IsInside(jhit.GetPosition().Theta()*TMath::RadToDeg(),
								jhit.GetEnergy())) continue;
					obj.FillHistogram(Form("doppler_%s",cut->GetName()),1000,0,3000,
							shit.GetDoppler(0.08,jhit.GetPosition()));
				}
			}
		}
	}
}


// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {

	TSega* sega = obj.GetDetector<TSega>();
	TJanusDDAS* janus = obj.GetDetector<TJanusDDAS>();

	TList *gates = &(obj.GetGates());
	if(gates_loaded!=gates->GetSize()) {
		TIter iter(gates);
		while(TObject *obj = iter.Next()) {
			GCutG *gate = (GCutG*)obj;
			std::string tag = gate->GetTag();
			if(!tag.compare("kin")) {
				kin_gates.push_back(gate);
				std::cout << "kin: << " << gate->GetName() << std::endl;
			}
			gates_loaded++;
		}
	}


	if(janus) {
		MakeRawJanus(obj);
		MakeCalJanus(obj);
	}
	if(sega) 
		MakeSega(obj);



}
