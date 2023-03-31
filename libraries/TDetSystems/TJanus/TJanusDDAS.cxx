#include "TJanusDDAS.h"


#include <cassert>
#include <iostream>
#include <memory>
#include <algorithm>
#include <vector>

#include "TMath.h"
#include "TRandom.h"

#include "DDASDataFormat.h"
#include "JanusDataFormat.h"
#include "TRawEvent.h"
#include "TChannel.h"
#include "GValue.h"
#include "TReaction.h"
#include "TSRIM.h"


TJanusDDAS::TJanusDDAS() {
  Clear();
}

TJanusDDAS::~TJanusDDAS(){ }

void TJanusDDAS::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TJanusDDAS& janus = (TJanusDDAS&)obj;
  janus.primary_hits = primary_hits;
  janus.primary_single_hits = primary_single_hits;
  janus.primary_double_hits = primary_double_hits;

  janus.secondary_hits = secondary_hits;
  janus.secondary_singles = secondary_singles;
  janus.secondary_doubles = secondary_doubles;

  janus.tertiary_hits = tertiary_hits;
  janus.tertiary_singles = tertiary_singles;
  janus.tertiary_doubles = tertiary_doubles;
}

void TJanusDDAS::Clear(Option_t* opt){
  TDetector::Clear(opt);
  //janus_channels.clear();
  //janus_hits.clear();
  ClearChannels();
  ClearCorrelatedHits();
}

void TJanusDDAS::ClearChannels() {

  janus_channels.clear();
  bad_charge_channels.clear();

  channels.clear();
  rings.clear();
  sectors.clear();

  primary_unused_channels.clear();
  primary_unused_rings.clear();
  primary_unused_sectors.clear();

  secondary_unused_channels.clear();
  secondary_unused_rings.clear();
  secondary_unused_sectors.clear();

  tertiary_unused_channels.clear();
  tertiary_unused_rings.clear();
  tertiary_unused_sectors.clear();

}

void TJanusDDAS::ClearCorrelatedHits()  {
  primary_hits.clear();
  primary_single_hits.clear();
  primary_double_hits.clear();

  secondary_hits.clear();
  secondary_singles.clear();
  secondary_doubles.clear();

  tertiary_hits.clear();
  tertiary_singles.clear();
  tertiary_doubles.clear();
  
}

int TJanusDDAS::BuildHits(std::vector<TRawEvent>& raw_data){
  UnpackChannels(raw_data);
  BuildCorrelatedHits();
  
  /*
     printf("**************************\n");
     printf("**************************\n");
     printf("**************************\n");
     Print("channels");
     Print();
     printf("**************************\n");
     printf("**************************\n");
     printf("**************************\n");
     */
  return primary_hits.size() + janus_channels.size();
}

void TJanusDDAS::UnpackChannels(std::vector<TRawEvent>& raw_data) {

  long smallest_timestamp = 0x7fffffffffffffff;
  for(auto& event : raw_data){
    //SetTimestamp(event.GetTimestamp());
    TSmartBuffer buf = event.GetPayloadBuffer();
    TDDASEvent<DDASHeader> ddas(buf);

    //std::cout << "In Janus Unpack Channels" << std::endl;

    unsigned int address = ( (5<<24) +
        (ddas.GetCrateID()<<16) +
        (ddas.GetSlotID()<<8) +
        ddas.GetChannelID() );
    TChannel* chan = TChannel::GetChannel(address);

    static int lines_displayed = 0;
    if(!chan){
      if(lines_displayed < 10) {
        std::cout << "Unknown JANUS (crate, slot, channel): ("
          << ddas.GetCrateID() << ", " << ddas.GetSlotID()
          << ", " << ddas.GetChannelID() << ")" << std::endl;
      }
      lines_displayed++;
      continue;
    }

    // bool is_ring = (*chan->GetArraySubposition() == 'F');
    // int detnum = chan->GetArrayPosition();
    // int segnum = chan->GetSegment();

    TJanusDDASHit janus_chan;
    janus_chan.SetCharge(ddas.GetEnergy());
    janus_chan.SetTime(ddas.GetCFDTime());
    janus_chan.SetTimestamp(ddas.GetTimestamp());
    if(janus_chan.Timestamp()<smallest_timestamp) { smallest_timestamp = janus_chan.Timestamp(); }
    janus_chan.SetAddress(address);

    janus_channels.push_back(janus_chan);
  }

  SetTimestamp(smallest_timestamp);
}


void TJanusDDAS::BuildCorrelatedHits() {
  ClearCorrelatedHits();

  //software thresholds; they're a little arbitrary
  //int minimum_charge = 1000;
  //int maximum_charge = 30000;

  int minimum_charge = GValue::Value("GLOBAL_THRESH_LOW");
  int maximum_charge = GValue::Value("GLOBAL_THRESH_HIGH");

  //32768 is max ADC number, 10000 (ns) was the width of the NSCL Event Builder window
  //These get changed later
  //double EWin = 33000; // dE/E condition
  //double TDiff = 11000; // timimg window (ns)

  double EWin = GValue::Value("EWIN");
  double TDiff = GValue::Value("TDIFF");

  for(size_t x=0;x<janus_channels.size();x++){
    TJanusDDASHit chan(janus_channels.at(x));
    if(chan.Charge() < minimum_charge || chan.Charge() > maximum_charge) {
      bad_charge_channels.push_back(chan);
      continue;
    }

    channels.push_back(chan);
    if(chan.IsRing()){
      rings.push_back(chan);
    }
    else if(chan.IsSector()){
      sectors.push_back(chan);
    }
  }

  //if(janus_channels.size() != bad_charge_channels.size() + channels.size())
  //{std::cout << "Bad counting 1" << std::endl;}

  std::sort(channels.begin(),channels.end());
  std::sort(rings.begin(),rings.end());
  std::sort(sectors.begin(),sectors.end());

  //if(channels.size() != rings.size() + sectors.size())
  //{std::cout << "Bad counting 2" << std::endl;}

  std::vector<bool> used_rings;
  std::vector<bool> used_sectors;

  //Ensure we only use each ring and sector once
  used_rings.resize(rings.size());
  std::fill(used_rings.begin(),used_rings.end(),false);

  used_sectors.resize(sectors.size());
  std::fill(used_sectors.begin(),used_sectors.end(),false);

  /*
  int used_chans = 0;
  int num_rings = 0;
  int num_sectors = 0;
  int remaining_chans = sectors.size() + rings.size();

  if(remaining_chans != channels.size())
    {std::cout << "Bad Counting 4" << std::endl;}
  */

  //std::vector<int> sect_index;
  //std::vector<int> ring_index;

  //long Tref;
  //if(sectors.size())
  //{Tref = sectors.at(0).Timestamp();}
  const int max_lvl = 3;
  for(int lvl=1;lvl<max_lvl+1;lvl++) {

    if(lvl==1) { //primary hits
      EWin=0.1;
    }
    else if(lvl==2) { //secondary hits
      EWin=0.15;
    }
    else if(lvl==3) { //tertiary hits
      EWin=0.2;
    }
    
    //Loop over sectors and rings. For each sector, pair with first unused ring that is in the same detector and meets the energy and time conditions. Note that rings and sectors are sorted by decreasing energy.
    for(size_t j=0;j<sectors.size();j++){
      if(used_sectors.at(j) /*|| std::fabs(sectors.at(j).Timestamp() - Tref) >  TDiff*/)
	{continue;}
      for(size_t i=0;i<rings.size();i++) {
	if(!used_rings.at(i) && rings.at(i).GetDetnum() == sectors.at(j).GetDetnum()
	   //&& std::fabs(rings.at(i).Timestamp() - Tref) <  TDiff
	   && std::fabs(rings.at(i).Charge() - sectors.at(j).Charge())/((double)sectors.at(j).Charge()) < EWin
	   && std::fabs(rings.at(i).Timestamp() - sectors.at(j).Timestamp()) < TDiff
	   ) {

	  //check min/max charge, detector, Energy ratio, only use channels once
	  MakeHit(rings.at(i),sectors.at(j),lvl,false,false);
	  used_rings.at(i) = true;
	  used_sectors.at(j) = true;
	  break;

	  /*
	  ring_index.push_back(i);
	  sect_index.push_back(j);
	  used_chans+=2;
	  num_rings+=1;
	  num_sectors+=1;
	  remaining_chans-=2;
	  */
        
	   
	}//end MakeHit conditions
      }//end ring loop  
    }//end sector loop

    /*
    if(janus_single_hits.size()*2 != used_chans)
      {std::cout << "Bad counting 5" << std::endl;}

    if(remaining_chans != channels.size() - used_chans)
      {std::cout << "Bad counting 7" << std::endl;}
    */
  
    //Trying to deal with two particles in the same ring (different sectors)
    bool broken = false;
    for(size_t j=0;j<sectors.size();j++){
      broken=false;
      if(used_sectors.at(j) /*|| std::fabs(sectors.at(j).Timestamp() - Tref) >  TDiff*/
	 || sectors.at(j).Charge() < GValue::Value("TWOHIT_THRESH_SEC"))
	{continue;}
      for(size_t i=0;i<rings.size();i++) {
	if(used_rings.at(i) || rings.at(i).Charge() < GValue::Value("TWOHIT_THRESH_RING") //ring charge cut
	 //|| std::fabs(rings.at(i).Timestamp() - Tref) >  TDiff
	   || rings.at(i).GetDetnum() != sectors.at(j).GetDetnum()
	   || std::fabs(rings.at(i).Timestamp() - sectors.at(j).Timestamp()) > TDiff)
	  {continue;}
	for(size_t k=j+1;k<sectors.size();k++) {
	  if(!used_sectors.at(k) //&& std::fabs(sectors.at(k).Timestamp() - Tref) <  TDiff
	     && sectors.at(k).Charge() > GValue::Value("TWOHIT_THRESH_SEC") //sector charge cut
	     && rings.at(i).GetDetnum() == sectors.at(k).GetDetnum()
	     && std::fabs(sectors.at(j).GetSector() - sectors.at(k).GetSector()) > 1
	     && std::fabs(sectors.at(j).GetSector() - sectors.at(k).GetSector()) < 31
	     && std::fabs(rings.at(i).Timestamp() - sectors.at(k).Timestamp()) < TDiff
	     && std::fabs(sectors.at(j).Timestamp() - sectors.at(k).Timestamp()) < TDiff
	     && std::fabs(sectors.at(k).Charge() + sectors.at(j).Charge() - rings.at(i).Charge())
	        /(double)rings.at(i).Charge() < EWin
	     ) {

	    MakeHit(rings.at(i),sectors.at(j),lvl,true,false);
	    MakeHit(rings.at(i),sectors.at(k),lvl,true,false);
	    used_rings.at(i) = true;
	    used_sectors.at(j) = true;
	    used_sectors.at(k) = true;
	    broken = true;
	    break;

	    /*
	    ring_index.push_back(i);
	    sect_index.push_back(j);
	    sect_index.push_back(k);
	    used_chans+=3;
	    num_rings+=1;
	    num_sectors+=2;
	    remaining_chans-=3;
	    */
	     
	  }//end MakeHit Conditions
	}//end second sector loop
	if(broken)
	  {break;}
      }//end ring loop
    }//end sector loop

    //Trying to handle charge-sharing between rings
    //Simple addback procedure (for rings)
    broken=false;
    for(size_t j=0;j<sectors.size();j++) {
      broken=false;
      if(used_sectors.at(j) /*|| std::fabs(sectors.at(j).Timestamp() - Tref) >  TDiff*/)
	{continue;}
      for(size_t i=0;i<rings.size();i++) {
	if(used_rings.at(i) //|| std::fabs(rings.at(i).Timestamp() - Tref) >  TDiff
	   || rings.at(i).GetDetnum() != sectors.at(j).GetDetnum()
	   || std::fabs(rings.at(i).Timestamp() - sectors.at(j).Timestamp()) > TDiff)
	  {continue;}
	for(size_t k=i+1;k<rings.size();k++) {
	  if(!used_rings.at(k) //&& std::fabs(rings.at(k).Timestamp() - Tref) <  TDiff 
	     && rings.at(k).GetDetnum() == rings.at(i).GetDetnum()
	     && std::fabs(rings.at(k).Timestamp() - rings.at(i).Timestamp()) < TDiff
	     && std::fabs(rings.at(k).Timestamp() - sectors.at(j).Timestamp()) < TDiff
	     && std::fabs(rings.at(k).GetSector() - rings.at(i).GetSector()) == 1 //GetRing() doens't work on unpaired chans
	     && std::fabs(rings.at(i).Charge() + rings.at(k).Charge() - sectors.at(j).Charge())
	     /(double)sectors.at(j).Charge() < EWin
	    ) {

	    //"Addback ring", charges of two rings are added together
	    //Higher energy ring is used for position and time
	    TJanusDDASHit ring_chan;
	    ring_chan.SetAddress(rings.at(i).Address());
	    ring_chan.SetTimestamp(rings.at(i).Timestamp());
	    ring_chan.SetTime(rings.at(i).Time());
	    ring_chan.SetCharge(rings.at(i).Charge() + rings.at(k).Charge());

	    MakeHit(ring_chan,sectors.at(j),lvl,false,true);
	    used_rings.at(i) = true;
	    used_rings.at(k) = true;
	    used_sectors.at(j) = true;
	    broken = true;
	    break;
            
	  } // end addback conditions 
	} //end second ring loop
	if(broken)
	  {break;}
      } //end first ring loop
    } //end sector loop
    
  
    /*
    for(int ind : sect_index) {
      if(used_sectors.at(ind) == false)
        {std::cout << "Boo1" << std::endl;}
    }

    if(janus_single_hits.size()*2 + janus_double_hits.size()*(3.0/2.0) != used_chans)
      {std::cout << "Bad counting 6" << std::endl;}

    if(remaining_chans != channels.size() - used_chans)
      {std::cout << "Bad counting 8" << std::endl;}
    */

    if(lvl==1) {
      for(size_t i=0;i<rings.size();i++) {
	if(!used_rings.at(i)) {
	  primary_unused_rings.push_back(rings.at(i));
	  primary_unused_channels.push_back(rings.at(i));
	}
      }
      for(size_t j=0;j<sectors.size();j++) {
	if(!used_sectors.at(j)) {
	  primary_unused_sectors.push_back(sectors.at(j));
	  primary_unused_channels.push_back(sectors.at(j));
	}
      }
    }
    
    else if(lvl==2) {
      for(size_t i=0;i<rings.size();i++) {
	if(!used_rings.at(i)) {
	  secondary_unused_rings.push_back(rings.at(i));
	  secondary_unused_channels.push_back(rings.at(i));
	}
      }
      for(size_t j=0;j<sectors.size();j++) {
	if(!used_sectors.at(j)) {
	  secondary_unused_sectors.push_back(sectors.at(j));
	  secondary_unused_channels.push_back(sectors.at(j));
	}
      }
    }

    else if(lvl==3) {
      for(size_t i=0;i<rings.size();i++) {
	if(!used_rings.at(i)) {
	  tertiary_unused_rings.push_back(rings.at(i));
	  tertiary_unused_channels.push_back(rings.at(i));
	}
      }
      for(size_t j=0;j<sectors.size();j++) {
	if(!used_sectors.at(j)) {
	  tertiary_unused_sectors.push_back(sectors.at(j));
	  tertiary_unused_channels.push_back(sectors.at(j));
	}
      }
    }

  } //end hit level loop

  AssignLevel(max_lvl);

  if(channels.size() != primary_single_hits.size()*2.0 + primary_double_hits.size()*(3.0/2.0) +
     + primary_addback_hits.size()*3.0 + primary_unused_channels.size())
    {std::cout << "Bad Primary hit/channel counting" << std::endl;}

  if(max_lvl > 1) {
    if(primary_unused_channels.size() != secondary_singles.size()*2.0 + secondary_doubles.size()*(3.0/2.0)
       + secondary_addback_hits.size()*3.0 + secondary_unused_channels.size())
      {std::cout << "Bad Secondary hit/channel counting" << std::endl;}
  }

  if(max_lvl > 2) {
    if(secondary_unused_channels.size() != tertiary_singles.size()*2.0 + tertiary_doubles.size()*(3.0/2.0)
       + tertiary_addback_hits.size()*3.0 + tertiary_unused_channels.size())
      {std::cout << "Bad Tertiary hit/channel counting" << std::endl;}  
  }
  
  /*
  if(sectors.size() - num_sectors != unused_sectors.size())
    {std::cout << "Bad Sector Count" << std::endl;}
  
  if(rings.size() - num_rings != unused_rings.size())
    {std::cout << "Bad Ring Count" << std::endl;}

  for(int ind : sect_index) {
    if(used_sectors.at(ind) == false)
      {std::cout << "Boo2" << std::endl;}

    //std::cout << "\nUsed Sector at index " << ind << "." << std::endl;;
  }

  for(int ind : ring_index) {
    if(used_rings.at(ind) == false)
      {std::cout << "Boo3" << std::endl;}

    //std::cout << "\nUsed Ring at index " << ind << "." << std::endl;;
  }

  for(int i=0;i<sect_index.size();i++) {
    for(int j=i+1;j<sect_index.size();j++) {
      if(sect_index.at(i)==sect_index.at(j)) {
	std::cout << "Sector at index " << sect_index.at(j) << " used twice" << std::endl;
      }
    }
  }

  for(int i=0;i<ring_index.size();i++) {
    for(int j=i+1;j<ring_index.size();j++) {
      if(ring_index.at(i)==ring_index.at(j)) {
	std::cout << "Ring at index " << sect_index.at(j) << " used twice" << std::endl;
      }
    }
  }
  */
  
  //not currently working
  /*
  if(false){

    if(rings.size()>2 || sectors.size()>2) return;

    janus_channels.clear();
    if(rings.size()==1) {
      janus_channels.push_back(rings[0]);
    }else if(rings.size()==2 && abs(rings.at(0).GetRing()-rings.at(1).GetRing())==1){
      TJanusDDASHit hit;
      if(rings.at(0).Charge()>rings.at(1).Charge()){
        rings.at(0).Copy(hit);
        hit.SetCharge(hit.Charge() + rings.at(1).Charge());
      }else {
        rings.at(1).Copy(hit);
        hit.SetCharge(hit.Charge() + rings.at(0).Charge());
      }
      janus_channels.push_back(hit);
    }else if(rings.size()!=0){
      janus_channels.push_back(rings[0]);
      janus_channels.push_back(rings[1]);
    }

    if(sectors.size()==1) {
      janus_channels.push_back(sectors[0]);
    }else if(sectors.size()==2 && ((abs(sectors.at(0).GetRing()-sectors.at(1).GetRing())==1) || 
          (abs(sectors.at(0).GetRing()-sectors.at(1).GetRing())==31))){
      TJanusDDASHit hit;
      if(sectors.at(0).Charge()>sectors.at(1).Charge()){
        sectors.at(0).Copy(hit);
        hit.SetCharge(hit.Charge() + sectors.at(1).Charge());
      }else {
        sectors.at(1).Copy(hit);
        hit.SetCharge(hit.Charge() + sectors.at(0).Charge());
      }
      janus_channels.push_back(hit);
    } else if(sectors.size()!=0){
      janus_channels.push_back(sectors[0]);
      janus_channels.push_back(sectors[1]);
    } 

  } //end if(false)
  */
}

void TJanusDDAS::MakeHit(const TJanusDDASHit& chan_ring, const TJanusDDASHit& chan_sector, const int level, const bool dbl,
			 const bool ab) {
  
  TJanusDDASHit correlated_hit(chan_sector);  

  TDetectorHit& back = correlated_hit.GetBackHit();
  back.SetCharge(chan_ring.Charge());
  back.SetTime(chan_ring.Time());
  back.SetAddress(chan_ring.Address());
  back.SetTimestamp(chan_ring.Timestamp());

  all_hits.push_back(correlated_hit);

  if(!dbl && !ab)
    {all_single_hits.push_back(correlated_hit);}
  else if(!dbl && ab)
    {all_addback_hits.push_back(correlated_hit);}
  else if(dbl)
    {all_double_hits.push_back(correlated_hit);}

  if(level==1) {
    primary_hits.push_back(correlated_hit);
    
    if(!dbl && !ab)
      {primary_single_hits.push_back(correlated_hit);}
    else if(!dbl && ab)
      {primary_addback_hits.push_back(correlated_hit);}
    else if(dbl)
      {primary_double_hits.push_back(correlated_hit);}
  }

  else if(level==2) {
    secondary_hits.push_back(correlated_hit);
    
    if(!dbl && !ab)
      {secondary_singles.push_back(correlated_hit);}
    else if(!dbl && ab)
      {secondary_addback_hits.push_back(correlated_hit);}
    else if(dbl)
      {secondary_doubles.push_back(correlated_hit);}
  }

  else if(level==3) {
    tertiary_hits.push_back(correlated_hit);
    
    if(!dbl && !ab)
      {tertiary_singles.push_back(correlated_hit);}
    else if(!dbl && ab)
      {tertiary_addback_hits.push_back(correlated_hit);}
    else if(dbl)
      {tertiary_doubles.push_back(correlated_hit);}
  }
  
  else {
    std::cout << "Undefined hit level (" << level << "). The hit was not made" << std::endl;
  }

  fSize++;
}

TJanusDDASHit& TJanusDDAS::GetJanusHit(int i){
  return all_hits.at(i);
}

TJanusDDASHit& TJanusDDAS::GetJanusChannel(int i){
  return janus_channels.at(i);
}

TDetectorHit& TJanusDDAS::GetHit(int i){
  return all_hits.at(i);
}

std::vector<TJanusDDASHit> TJanusDDAS::GetDetectorChannels(const int detNum) {
  std::vector<TJanusDDASHit> DetectorChannels;

  for(auto chan : channels) {
    if(chan.GetDetnum() == detNum) {
      DetectorChannels.push_back(chan);
    }
  }
  return DetectorChannels;
}

std::vector<TJanusDDASHit>& TJanusDDAS::GetUnusedChannels(const int lvl) {

  if(lvl==0 || lvl==3)
    {return tertiary_unused_channels;}
  
  else if(lvl==1)
    {return primary_unused_channels;}
  
  else if(lvl==2)
    {return secondary_unused_channels;}

  else {
    std::cout << "Unkown hit level (" << lvl << "). Returning primary unused channels." << std::endl;
    return primary_unused_channels;
  }

}

std::vector<TJanusDDASHit>& TJanusDDAS::GetUnusedRings(const int lvl) {
  
  if(lvl==0 || lvl==3)
    {return tertiary_unused_rings;}
  
  else if(lvl==1)
    {return primary_unused_rings;}
  
  else if(lvl==2)
    {return secondary_unused_rings;}

  else {
    std::cout << "Unkown hit level (" << lvl << "). Returning primary unused rings." << std::endl;
    return primary_unused_rings;
  }

}

std::vector<TJanusDDASHit>& TJanusDDAS::GetUnusedSectors(const int lvl) {

  if(lvl==0 || lvl==3)
    {return tertiary_unused_sectors;}
  
  else if(lvl==1)
    {return primary_unused_sectors;}
  
  else if(lvl==2)
    {return secondary_unused_sectors;}

  else {
    std::cout << "Unkown hit level (" << lvl << "). Returning primary unused sectors." << std::endl;
    return primary_unused_sectors;
  }

}

std::vector<TJanusDDASHit>& TJanusDDAS::GetHits(const int lvl) {

  if(lvl==0)
    {return all_hits;}
  
  else if(lvl==1)
    {return primary_hits;}
  
  else if(lvl==2)
    {return secondary_hits;}

  else if (lvl==3)
    {return tertiary_hits;}

  else {
    std::cout << "Unkown hit level (" << lvl << "). Returning primary hits." << std::endl;
    return primary_hits;
  }

}

std::vector<TJanusDDASHit>& TJanusDDAS::GetSingleHits(const int lvl) {

  if(lvl==0)
    {return all_single_hits;}
  
  else if(lvl==1)
    {return primary_single_hits;}
  
  else if(lvl==2)
    {return secondary_singles;}

  else if (lvl==3)
    {return tertiary_singles;}

  else {
    std::cout << "Unkown hit level (" << lvl << "). Returning primary singles." << std::endl;
    return primary_single_hits;
  }

}

std::vector<TJanusDDASHit>& TJanusDDAS::GetDoubleHits(const int lvl) {

  if(lvl==0)
    {return all_double_hits;}
  
  else if(lvl==1)
    {return primary_double_hits;}
  
  else if(lvl==2)
    {return secondary_doubles;}

  else if (lvl==3)
    {return tertiary_doubles;}

  else {
    std::cout << "Unkown hit level (" << lvl << "). Returning primary doubles." << std::endl;
    return primary_double_hits;
  }

}

std::vector<TJanusDDASHit>& TJanusDDAS::GetAddbackHits(const int lvl) {
  
  if(lvl==0) {
    return all_addback_hits;
  }
  
  else if(lvl==1) {
    return primary_addback_hits;
  }
  
  else if(lvl==2) {
    return secondary_addback_hits;
  }
  
  else if(lvl==3) {
    return tertiary_addback_hits;
  }

  else {
    std::cout << "Unkown hit level (" << lvl << "). Returning all addback hits." << std::endl;
    return all_addback_hits;
  }

}

std::vector<TJanusDDASHit> TJanusDDAS::GetDetectorHits(const int detNum, const int lvl) {
  std::vector<TJanusDDASHit> DetectorHits;

  if(lvl==0) {
    for(auto hit : all_hits) {
      if(hit.GetDetnum() == detNum) {
	DetectorHits.push_back(hit);
      }
    }
  }

  else if(lvl==1) {
    for(auto hit : primary_hits) {
      if(hit.GetDetnum() == detNum) {
	DetectorHits.push_back(hit);
      }
    }
  }
  
  else if(lvl==2) {
    for(auto hit : secondary_hits) {
      if(hit.GetDetnum() == detNum) {
	DetectorHits.push_back(hit);
      }
    }
  }

  else if(lvl==3) {
    for(auto hit : tertiary_hits) {
      if(hit.GetDetnum() == detNum) {
	DetectorHits.push_back(hit);
      }
    }
  }
  
  else {
    std::cout << "Unkown hit level (" << lvl << ") or detNum (" << detNum << "). This didn't work" << std::endl;
  }

  return DetectorHits;
}

std::vector<TJanusDDASHit> TJanusDDAS::GetSpecificHits(const int detNum, const int lvl, const int typ) {
  std::vector<TJanusDDASHit> DesiredHits;

  if(typ==0) {
    for(auto hit : GetDetectorHits(detNum,lvl)) {
      DesiredHits.push_back(hit);
    }
  }

  else if (typ==1) {
    for(auto hit : GetSingleHits(lvl)) {
      if(hit.GetDetnum() == detNum) {
        DesiredHits.push_back(hit);
      }
    }
  }

  else if (typ==2) {
    for(auto hit : GetDoubleHits(lvl)) {
      if(hit.GetDetnum() == detNum) {
        DesiredHits.push_back(hit);
      }
    }
  }

  else if (typ==3) {
    for(auto hit : GetAddbackHits(lvl)) {
      if(hit.GetDetnum() == detNum) {
        DesiredHits.push_back(hit);
      }
    }
  }

  else {
    std::cout << "Unkown hit type (" << typ << "). This didn't work"
	      << std::endl;
  }

  return DesiredHits;

}

double TJanusDDAS::GetBeta(double theta, TReaction& reac, int part, TSRIM& srim, bool sol2,
			   bool dE) {

  if(sol2)
    {theta *= -1;}
  
  double post_energy_MeV = reac.GetTLab(theta,part);

  // Factors of 1e3 are because TNucleus and TReaction use MeV, while TSRIM uses keV
  if(dE) {

    double thickness;
    // (mg/cm^2) / (mg/cm^3) * (10^4 um/cm)
    if(std::isnan(GValue::Value("TARGET_THICKNESS")) || std::isnan(GValue::Value("TARGET_DENSITY"))) {
      thickness = (0.92/11342.0) * 10000; //standard lead target
    }
    else {
      thickness = (GValue::Value("TARGET_THICKNESS") / GValue::Value("TARGET_DENSITY")) * 10000.0;
    }
    
    double distance_travelled = (thickness/2.0)/std::abs(std::cos(theta));
    post_energy_MeV = srim.GetAdjustedEnergy(post_energy_MeV*1e3,distance_travelled)/1e3;
    
  }
  
  return reac.AnalysisBeta(post_energy_MeV,part);
  
}

TVector3 TJanusDDAS::GetPosition(int detnum, int ring_num, int sector_num, bool before_e17507B){
  if(detnum<0 || detnum>1 ||
      ring_num<1 || ring_num>24 ||
      sector_num<1 || sector_num>32){
    // batman vector, nan-nan-nan
    return TVector3(std::sqrt(-1),std::sqrt(-1),std::sqrt(-1));
  }

  //const double PI = 3.1415926535;
  const double PI = TMath::Pi();
  
  double zoff;
  if(detnum == 1)
    {zoff = 2.8;}
  else
    {zoff = 3.2;}
  TVector3 origin = TVector3(0,0,zoff);
  
  double phi_offset; // Phi of sector 1 of downstream detector
  bool clockwise; // Winding direction of sectors.
  if(before_e17507B) {
    //phi_offset = 2.*3.1415926535*(0.25);
    phi_offset = 0.5*PI;
    clockwise = true;
  }
  else if(!before_e17507B) {
    //phi_offset = 2.*PI*(0.25)+PI;
    phi_offset = 1.5*PI;
    clockwise = false;
  }

  double janus_outer_radius = 3.5;
  double janus_inner_radius = 1.1;

  TVector3 position(1.,0,0);  // Not (0,0,0), because otherwise SetPerp and SetPhi fail.
  double rad_slope = (janus_outer_radius - janus_inner_radius) /24.;
  double rad_offset = janus_inner_radius;
  double perp_num = ring_num - 0.5; // Shift to 0.5-23.5
  position.SetPerp(perp_num*rad_slope + rad_offset);
  double phi_num = sector_num;
  double phi =
    phi_offset +
    (clockwise ? -1 : 1) * 2.*3.1415926/32. * (phi_num - 1);
  position.SetPhi(phi);

  position += origin;

  if(detnum==0){
    position.RotateY(TMath::Pi());
  }

  return position;
}

TVector3 TJanusDDAS::GetReconPosition(double theta_det, double phi_det, TReaction& reaction, int det_part, int recon_part,
				      bool sol2) {
  
  //double theta = Reconstruct(theta_det,beamname,targetname,sfile);
  double theta = Reconstruct(theta_det,reaction,det_part,recon_part,sol2);
  double phi = phi_det-TMath::Pi();

  TVector3 v;
  v.SetMagThetaPhi(1,theta,phi);
  return v;
}

double TJanusDDAS::Reconstruct(double theta, TReaction& reaction, int det_pt, int recon_pt, bool s2) {
  
  if(s2)
    {theta*=-1;}
  return reaction.ConvertThetaLab(theta,det_pt,recon_pt);
  
}

/*
TVector3 TJanusDDAS::GetReconPosition(double theta_det, double phi_det, int det_part, int recon_part,
				      bool sol2) {
  
  double theta = Reconstruct(theta_det,det_part,recon_part,sol2);
  double phi = phi_det-TMath::Pi();

  TVector3 v;
  v.SetMagThetaPhi(1,theta,phi);
  return v;
}

double TJanusDDAS::Reconstruct(double theta, int det_pt, int recon_pt, bool s2) {
  
  if(s2)
    {theta*=-1;}
  return reac.ConvertThetaLab(theta,det_pt,recon_pt);
  
}
*/
/*
double TJanusDDAS::Reconstruct(double theta, const char *beamname, const char *targetname, const char *srimfile) {
  
  static auto beam = std::make_shared<TNucleus>(beamname);
  static auto targ = std::make_shared<TNucleus>(targetname);
  static TSRIM srim(srimfile); 
  double thickness = ( GValue::Value("targetthick") / 11342.0) * 1e4; // (0.75 mg/cm^2) / (11342 mg/cm^3) * (10^4 um/cm)

  double energy_mid = srim.GetAdjustedEnergy(GValue::Value("beamenergy")*1e3, thickness*0.5)/1e3;

  TReaction reac_mid(beam, targ, beam, targ, energy_mid);

  //return beam based on target (give 3, calculate 2).
  return reac_mid.ConvertThetaLab(theta,3,2);
}
*/

double TJanusDDAS::LabSolidAngle(int detNum, int RingNum) {
  
  if(detNum<0 || detNum>1 ||
     RingNum<1 || RingNum>24) {
    return -1;
  }

  //needlessly precise values
  //sr
  std::vector<double> DS_Ring_Solid_Angles = {//R1         //R2 ....
                                              0.081730418, 0.085025760, 0.087688448, 0.089758318, 0.091278095, 0.09229107,
					      0.092849394, 0.092996923, 0.092785682, 0.092250983, 0.091459302, 0.09041001,
					      0.089183532, 0.087775470, 0.086237347, 0.084606860, 0.082886524, 0.08109079,
					      0.079249816, 0.077393763, 0.075532684, 0.073632020, 0.071775967, 0.069904206
                                             };

  std::vector<double> US_Ring_Solid_Angles = {//R1         //R2 ....
                                              0.048180973, 0.051078904, 0.053716208, 0.056095085, 0.058215785, 0.060085033,
                                              0.061710744, 0.063097695, 0.064261969, 0.065208908, 0.065955224, 0.066510344,
                                              0.066890351, 0.067106932, 0.067172403, 0.067099707, 0.066902415, 0.066591083,
                                              0.066178152, 0.065674429, 0.065090721, 0.064435762, 0.063715834, 0.062947589
                                             };

  if(detNum==1)
    {return DS_Ring_Solid_Angles.at(RingNum-1);}
  else if(detNum==0)
    {return US_Ring_Solid_Angles.at(RingNum-1);}
  else
    return -1;
 
}


double TJanusDDAS::CmSolidAngle(int detNum, int RingNum, TReaction& reaction, int part, bool before, bool maxRingCut, bool s2) {
  if(detNum<0 || detNum>1 ||
     RingNum<1 || RingNum>24) {
    return -1;
  }

  if(maxRingCut) {
    if(RingNum > 2) {
      RingNum = 2;
    }
  }

  //the same as GetPosition(), but now we want the edges of the ring, not the middle
  const double PI = TMath::Pi();
  
  double zoff;
  if(detNum == 1)
    {zoff = 2.8;}
  else
    {zoff = 3.2;}
  TVector3 zpos = TVector3(0,0,zoff);

  double janus_outer_radius = 3.5;
  double janus_inner_radius = 1.1;

  double rad_slope = (janus_outer_radius - janus_inner_radius)/24.;
  double rad_offset = janus_inner_radius;

  TVector3 position1(1.,0,0);  // Not (0,0,0), because otherwise SetPerp and SetPhi fail.
  position1.SetPerp((RingNum-1)*rad_slope + rad_offset);
  if(before) {
    position1.SetPhi(0.5*PI);
  }
  else if(!before) {
    position1.SetPhi(1.5*PI);
  }
  position1+=zpos;

  TVector3 position2(1.,0,0);
  position2.SetPerp(RingNum*rad_slope + rad_offset);
  if(before) {
    position2.SetPhi(0.5*PI);
  }
  else if(!before) {
    position2.SetPhi(1.5*PI);
  }
  position2+=zpos;

  if(detNum==0){
    position1.RotateY(PI);
    position2.RotateY(PI);
  }

  //integrate sin(theta) from theta1 to theta2 in the CM frame
  double theta1;
  double theta2;
 
  if(!s2) {
    theta1 = reaction.ConvertThetaLabToCm(position1.Theta(),part);
    theta2 = reaction.ConvertThetaLabToCm(position2.Theta(),part);
  }
  else {
    theta1 = reaction.ConvertThetaLabToCm(position1.Theta()*(-1.0),part);
    theta2 = reaction.ConvertThetaLabToCm(position2.Theta()*(-1.0),part);
  }
  
  //double term1 = TMath::Cos(theta1);
  //double term2 = TMath::Cos(theta2);
  
  //two pi coverage in phi, keep terms straight
  //if(detNum == 1) 
  //{return 2*PI*(term1 - term2);}
  //else
  //{return 2*PI*(term2 - term1);}

  //return std::fabs(2*PI*(term1 - term2));

  return std::fabs(2*PI*(TMath::Cos(theta1) - TMath::Cos(theta2)));
}
/*
double TJanusDDAS::CmSolidAngle(int detNum, int RingNum, int part, bool before) {
  if(detNum<0 || detNum>1 ||
     RingNum<1 || RingNum>24) {
    return -1;
  }

  const double PI = TMath::Pi();
  
  double zoff;
  if(detNum == 1)
    {zoff = 2.50;}
  else
    {zoff = 3.13;}
  TVector3 zpos = TVector3(0,0,zoff);

  double janus_outer_radius = 3.5;
  double janus_inner_radius = 1.1;

  double rad_slope = (janus_outer_radius - janus_inner_radius)/24.;
  double rad_offset = janus_inner_radius;

  TVector3 position1(1.,0,0);  // Not (0,0,0), because otherwise SetPerp and SetPhi fail.
  position1.SetPerp((RingNum-1)*rad_slope + rad_offset);
  if(before) {
    position1.SetPhi(0.5*PI);
  }
  else if(!before) {
    position1.SetPhi(1.5*PI);
  }
  position1+=zpos;

  TVector3 position2(1.,0,0);
  position2.SetPerp(RingNum*rad_slope + rad_offset);
  if(before) {
    position2.SetPhi(0.5*PI);
  }
  else if(!before) {
    position2.SetPhi(1.5*PI);
  }
  position2+=zpos;

  if(detNum==0){
    position1.RotateY(PI);
    position2.RotateY(PI);
  }

  //integrate sin(theta) from theta1 to theta2, in the CM frame
  double theta1 = reac.ConvertThetaLabToCm(position1.Theta(),part);
  double theta2 = reac.ConvertThetaLabToCm(position2.Theta(),part);
  
  double term1 = TMath::Cos(theta1);
  double term2 = TMath::Cos(theta2);
  
  //two pi coverage in phi, keep terms straight
  if(detNum == 1) 
    {return 2*PI*(term1 - term2);}
  else
    {return 2*PI*(term2 - term1);}
}
*/
void TJanusDDAS::InsertHit(const TDetectorHit& hit) {
  primary_hits.emplace_back((TJanusDDASHit&)hit);
  fSize++;
}

void TJanusDDAS::Print(Option_t *opt) const {
  TString sopt(opt);
  if(sopt.Contains("channels")){
    PrintChannels(opt); 
  } else {
    PrintHits(opt); 
  }
}


void TJanusDDAS::PrintHits(Option_t *opt) const {
  printf("TJanusDDAS @ %lu\n",Timestamp());
  printf(" Size: %zu\n",primary_hits.size());
  for(unsigned int i=0;i<Size();i++) {
    printf("\t"); primary_hits.at(i).Print(); printf("\n");
  }
  printf("---------------------------\n");
  fflush(stdout);
}

void TJanusDDAS::PrintChannels(Option_t *opt) const {
  printf("JANUS Channels!\n");
  printf("TJanusDDAS @ %lu\n",Timestamp());
  printf(" Size: %zu\n",janus_channels.size());
  for(unsigned int i=0;i<janus_channels.size();i++) {
    TJanusDDASHit hit = janus_channels.at(i);
    TChannel *c = TChannel::GetChannel(hit.Address());
    if(c) {
      printf("\t%s\tDet: %i \tChannel: %02i\tChg: %i\tTime: %lu\n",
          c->GetName(),hit.GetDetnum(),hit.GetFrontChannel(),hit.Charge(),hit.Timestamp());
    } else {
      printf("\tjanus_channel 0x%08x, no channel.\n",hit.Address());
    }
  }
  printf("---------------------------\n");
  fflush(stdout);
}
