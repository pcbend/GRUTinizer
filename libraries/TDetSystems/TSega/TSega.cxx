#include "TSega.h"

#include <algorithm>
#include <iostream>
#include <fstream>

#include "DDASDataFormat.h"
#include "TNSCLEvent.h"
#include "TChannel.h"
#include "TRawEvent.h" // added Mark

std::map<int,TSega::Transformation> TSega::detector_positions;
std::map<std::array<int,2>,int> TSega::seg_map;
std::map<std::array<int,2>,int> TSega::pair_map;
std::map<std::array<int,2>,int> TSega::slice_map;

TSega::TSega(){ }

TSega::~TSega(){ }

/*******************************************************************************/
/* Copies hit ******************************************************************/
/*******************************************************************************/
void TSega::Copy(TObject& obj) const {
  TDetector::Copy(obj);
  TSega& sega = (TSega&)obj;
  sega.sega_hits = sega_hits;
}

/*******************************************************************************/
/* Clear hit *******************************************************************/
/*******************************************************************************/
void TSega::Clear(Option_t* opt){
  TDetector::Clear(opt);
  sega_hits.clear();
}

/*******************************************************************************/
/* Functions to call SeGA hits *************************************************/
/*******************************************************************************/
TSegaHit& TSega::GetSegaHit(int i){
  return sega_hits.at(i);
}

TDetectorHit& TSega::GetHit(int i){
  return sega_hits.at(i);
}

/*******************************************************************************/
/* Unpacks raw DDAS data and builds TSega events *******************************/
/*******************************************************************************/
int TSega::BuildHits(std::vector<TRawEvent>& raw_data) {

  long int smallest_timestamp = 0x7fffffffffffffff;
  for(auto& event : raw_data){
    //Unpack raw data into a DDAS Event
    TSmartBuffer buf = event.GetPayloadBuffer();
    TDDASEvent<DDASHeader> ddas(buf);
    unsigned int address = ( (1<<24) + (ddas.GetCrateID()<<16) + (ddas.GetSlotID()<<8) + ddas.GetChannelID() );
    //If channel not found in channels.cal file skip and do nothing
    TChannel* chan = TChannel::GetChannel(address);
    static int lines_displayed = 0;
    if(!chan){
      if(lines_displayed < 10 && ddas.GetCrateID() !=3) {
        std::cout << "Unknown SeGA (crate, slot, channel): (" << ddas.GetCrateID() << ", " << ddas.GetSlotID() << ", " << ddas.GetChannelID() << ")" << std::endl;
      }
      lines_displayed++;
      continue;
    }
    //Get detector information from channels.cal file
    int detnum = chan->GetArrayPosition();
    int segnum = chan->GetSegment();
    // Get a hit, make it if it does not exist
    TSegaHit* hit = NULL;
    for(auto& ihit : sega_hits){
      if(ihit.GetDetnum() == detnum) {
	if(segnum == 0) {
	  if(!ihit.HasCore()){ //If there is no core event assign to the ihit
              hit = &ihit;
	      break;
          }
        } else {
	  int tdiff = static_cast<int>(ihit.Timestamp()-ddas.GetTimestamp());
	  if(tdiff < 2000 && tdiff > -2000) { //Segments should be within 2us of a core
            hit = &ihit;
      	    break;
	  } //else std::cout << "BadSegmentMatch " << detnum << "\t" << tdiff << std::endl;
        }
      }
    }
    //Make a new hit
    if(hit == NULL){
      sega_hits.emplace_back();
      hit = &sega_hits.back();
      fSize++;
    }

    //Is a core event
    if(segnum==0){
      hit->SetAddress(address);
      hit->SetTimestamp(ddas.GetTimestamp()); // Timestamp in ns
      hit->SetTimeFull(ddas.GetTime()); // Timestamp + CFD
      hit->SetCFDTime(ddas.GetCFDTime()); // CFD ONLY
      if(hit->Timestamp()<smallest_timestamp) { smallest_timestamp = hit->Timestamp(); }
      hit->SetCharge(ddas.GetEnergy());
      hit->SetTrace(ddas.GetTraceLength(), ddas.trace);
      //For checking Trapezoidal filter output in DDAS, unlikely to be useful for mosr people
      hit->SetEnergySumBool(ddas.HasEnergySum());
      hit->SetEnergySum1(ddas.GetEnergySum(0));
      hit->SetEnergySum2(ddas.GetEnergySum(1));
      hit->SetEnergySum3(ddas.GetEnergySum(2));
      hit->SetEnergySum4(ddas.GetEnergySum(3));
    } else {  //Segment hit, add to SegmentHit vector
      TSegaSegmentHit& seg = hit->MakeSegmentByAddress(address);
      if(!hit->HasCore()) hit->SetTimestamp(ddas.GetTimestamp()); //If no core present use segment for Timestamp
      seg.SetCharge(ddas.GetEnergy());
      seg.SetTimestamp(ddas.GetTimestamp());  // Timestamp in ns
      seg.SetTimeFull(ddas.GetTime()); // Timestamp + CFD
      seg.SetCFDTime(ddas.GetCFDTime()); // CFD ONLY
      seg.SetTrace(ddas.GetTraceLength(), ddas.trace);
    }
  }
  //set the TSeGA  time....
  SetTimestamp(smallest_timestamp);  //fix me pcb
  return Size();
}

/*******************************************************************************/
/* Gets interaction position based on detector and segment number **************/
/*******************************************************************************/
TVector3 TSega::GetSegmentPosition(int detnum, int segnum) {
  if(detnum < 1 || detnum > 16 || segnum < 1 || segnum > 32){
    return TVector3(std::sqrt(-1),std::sqrt(-1),std::sqrt(-1));
  }

  double segment_height = 1.0;
  double perp_distance = 1.5;

  // Middle of the segment
  double segment_phi = 3.1415926535/4.0;
  double segment_z = segment_height/2.0;

  double crystal_phi = segment_phi + (segnum-2)*3.1415926/2.0;
  double crystal_z = segment_z + ((segnum-1)/4)*segment_height;

  TVector3 crystal_pos(1,0,0);
  crystal_pos.SetZ(crystal_z);
  crystal_pos.SetPhi(crystal_phi);
  crystal_pos.SetPerp(perp_distance);
  TVector3 global_pos = CrystalToGlobal(detnum, crystal_pos);
  return global_pos;
}

/*******************************************************************************/
/* Read geometric maps from text files *****************************************/
/* I do not know the purpose of these function *********************************/
/*******************************************************************************/
void TSega::LoadSegmentMaps(){
  static bool maps_loaded = false;
  if (maps_loaded){
    return;
  }

  std::string fname = std::string(getenv("GRUTSYS")) + "/config/segment_map.txt";
  std::ifstream f(fname);

  if(!f) {
    std::cout << "SeGA segment map file \"" << fname << "\"" << " does not exist" << std::endl;
    return;
  }

  std::string line;
  while(std::getline(f,line)) {
    int det, seg, map;
    int extracted = sscanf(line.c_str(),"{{%i,%i},%i}",&det,&seg,&map);

    if(extracted != 3) {
      std::cout << "Segment Map: Extracted = " << extracted << std::endl;
    }
    seg_map[{det,seg}] = map;
  }

  std::string fname2 = std::string(getenv("GRUTSYS")) + "/config/pair_map.txt";
  std::ifstream f2(fname2);

  if(!f2) {
    std::cout << "SeGA segment pair map file \"" << fname2 << "\"" << " does not exist" << std::endl;
    return;
  }

  std::string line2;
  while(std::getline(f2,line2)) {
    int det2, seg2, map2;
    int extracted2 = sscanf(line2.c_str(),"{{%i,%i},%i}",&det2,&seg2,&map2);

    if(extracted2 != 3) {
      std::cout << "Pair Map: Extracted = " << extracted2 << std::endl;
    }
    pair_map[{det2,seg2}] = map2;
  }

  std::string fname4 = std::string(getenv("GRUTSYS")) + "/config/slice_map.txt";
  std::ifstream f4(fname4);

  if(!f4) {
    std::cout << "SeGA slice map file \"" << fname4 << "\"" << " does not exist" << std::endl;
    return;
  }

  std::string line4;
  while(std::getline(f4,line4)) {
    int det4, seg4, map4;
    int extracted4 = sscanf(line4.c_str(),"{{%i,%i},%i}",&det4,&seg4,&map4);

    if(extracted4 != 3) {
      std::cout << "Slice Map: Extracted = " << extracted4 << std::endl;
    }
    slice_map[{det4,seg4}] = map4;
  }
  maps_loaded = true;
  return;
}

/*******************************************************************************/
/* Returns information from maps loaded above **********************************/
/* I do not know the purpose of these function *********************************/
/*******************************************************************************/
int TSega::MappedSegnum(int detnum, int segnum) {
  LoadSegmentMaps();
  return seg_map[{detnum,segnum}];
}

int TSega::MappedPairnum(int detnum, int segnum) {
  LoadSegmentMaps();
  return pair_map[{detnum,segnum}];
}

int TSega::MappedSlicenum(int detnum, int segnum) {
  LoadSegmentMaps();
  return slice_map[{detnum,segnum}];
}

/*******************************************************************************/
/* Returns real position from detector number and position within crystals *****/
/*******************************************************************************/
TVector3 TSega::CrystalToGlobal(int detnum, TVector3 crystal_pos) {
  LoadDetectorPositions();
  static int lines_displayed = 0;
  if(!detector_positions.count(detnum)) {
    if(lines_displayed < 1000) {
      std::cout << "No transformation matrix loaded for SeGA det " << detnum << std::endl;
    }
    lines_displayed++;
    return TVector3(std::sqrt(-1),std::sqrt(-1),std::sqrt(-1));
  }
  Transformation& trans = detector_positions[detnum];
  return trans.origin + crystal_pos.X()*trans.x + crystal_pos.Y()*trans.y + crystal_pos.Z()*trans.z;
}

/*******************************************************************************/
/* Load in Rotation matrices used to calculate positions for *******************/
/* doppler corrections *********************************************************/
/*******************************************************************************/
void TSega::LoadDetectorPositions() {
  static bool loaded = false;
  if(loaded){
    return;
  }
  loaded = true;
  std::string filename = std::string(getenv("GRUTSYS")) + "/config/SeGA_rotations.txt";

  //Read the locations from file.
  std::ifstream infile(filename);
  if(!infile){	std::cout << "SeGA rotation matrix file \"" << filename << "\""	<< " does not exist, skipping" << std::endl;
    return;
  }

  std::string line;
  while (std::getline(infile,line)) {
    //Parse the line
    int detnum;
    char name_c[20];
    double x,y,z;
    int extracted = sscanf(line.c_str(),"det.%02d.%6s: %lf %lf %lf", &detnum,name_c,&x,&y,&z);
    if (extracted!=5) {
      continue;
    }

    //Pack into the vector of transformations.
    std::string name = name_c;
    TVector3 vec = TVector3(x,y,z);
    if (name=="origin") {
      detector_positions[detnum].origin = vec;
    } else if (name=="x_vect") {
      detector_positions[detnum].x = vec;
    } else if (name=="y_vect") {
      detector_positions[detnum].y = vec;
    } else if (name=="z_vect") {
      detector_positions[detnum].z = vec;
    }
  }
}

/*******************************************************************************/
/* Unused function *************************************************************/
/*******************************************************************************/
void TSega::InsertHit(const TDetectorHit& hit) {
  sega_hits.emplace_back((TSegaHit&)hit);
  fSize++;
}


/*******************************************************************************/
/* Specific to a single experiment *********************************************/
/* Will be modified to be more general or will be removed **********************/
/*******************************************************************************/
void TSega::SetRunStart(unsigned int unix_time) {
  // Only adjust times for production runs in e13701.
  if(unix_time < 1453953420 || unix_time > 1454425200) {
    return;
  }

  // Wed Jan 27 22:57:09 2016
  unsigned int previous = fRunStart==0 ? 1453953429 : fRunStart;
  int tdiff = unix_time - previous;
  long timestamp_diff = (1e9) * tdiff;
  fTimestamp += timestamp_diff;
  for(auto& hit : sega_hits) {
    hit.SetTimestamp(timestamp_diff + hit.Timestamp());
  }
}

/*******************************************************************************/
/* Required by TDetectorFactory ************************************************/
/*******************************************************************************/
void TSega::SortHitsByTimestamp() {
  std::sort(sega_hits.begin(), sega_hits.end(),	[](const TSegaHit& a, const TSegaHit& b) {
    return a.Timestamp() < b.Timestamp();
  });
}
