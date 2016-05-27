#include "TCAGRA.h"

#include <iostream>
#include <fstream>

#include "TANLEvent.h"
#include "TChannel.h"

#include "TGEBEvent.h"


std::map<int,TVector3> TCAGRA::detector_positions;


TCAGRA::TCAGRA(){
  Clear();
}

TCAGRA::~TCAGRA() {

}

void TCAGRA::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TCAGRA& detector = (TCAGRA&)obj;
  detector.cagra_hits = cagra_hits;
}

void TCAGRA::InsertHit(const TDetectorHit& hit){
  cagra_hits.emplace_back((TCAGRAHit&)hit);
  fSize++;
}

int TCAGRA::BuildHits(std::vector<TRawEvent>& raw_data){
  for (auto& event : raw_data) {
    SetTimestamp(event.GetTimestamp());

    auto buf = event.GetPayloadBuffer();
    TANLEvent anl(buf);

    unsigned int address = ( (1<<24) +
                             (anl.GetBoardID() << 8) +
                             anl.GetChannel() );

    TChannel* chan = TChannel::GetChannel(address);

    static int lines_displayed = 0;
    if(!chan){
      if(lines_displayed < 10) {
        std::cout << "Unknown (board id, channel): ("
                  << anl.GetBoardID() << ", " << anl.GetChannel()
                  << ")" << std::endl;
      } else if(lines_displayed==1000){
        std::cout << "I'm going to stop telling you that the channel was unknown,"
                  << " you should probably stop the program." << std::endl;
      }
      lines_displayed++;

      continue;
    }

    int detnum = chan->GetArrayPosition(); // clover number
    int segnum = chan->GetSegment(); // segment number

    // Get a hit, make it if it does not exist
    TCAGRAHit* hit = NULL;
    for(auto& ihit : cagra_hits){
      if(ihit.GetDetnum() == detnum){
        hit = &ihit;
        break;
      }
    }
    if(hit == NULL){
      cagra_hits.emplace_back();
      hit = &cagra_hits.back();
      fSize++;
    }

    if(segnum==0){
      hit->SetAddress(address);
      hit->SetTimestamp(event.GetTimestamp());
      hit->SetCharge(anl.GetEnergy());
    } else {
      TCAGRASegmentHit& seg = hit->MakeSegmentByAddress(address);
      seg.SetCharge(anl.GetEnergy());
      seg.SetTimestamp(event.GetTimestamp());
    }
  }



  //TCAGRAHit hit;
  //hit.BuildFrom(buf);
  //hit.SetTimestamp(event.GetTimestamp());
  //InsertHit(hit);
  return Size();
}
TVector3 TCAGRA::GetSegmentPosition(int detnum, char subpos, int segnum) {
  if(detnum < 1 || detnum > 16 || segnum < 0 || segnum > 2 ||
     subpos < 0x41 || subpos > 0x44){
    return TVector3(std::sqrt(-1),std::sqrt(-1),std::sqrt(-1));
  }
  LoadDetectorPositions();

  int index = (detnum << 16) + (((int)subpos) << 8) + segnum;

  if (detector_positions.count(index)>0) {
    return detector_positions.at(index);
  } else {
    return TVector3(0.,0.,0.);
  }

  // double segment_height = 1.0;
  // double perp_distance = 1.5;

  // // Middle of the segment
  // double segment_phi = 3.1415926535/4.0;
  // double segment_z = segment_height/2.0;

  // double crystal_phi = segment_phi + (segnum-2)*3.1415926/2.0;
  // double crystal_z = segment_z + ((segnum-1)/4)*segment_height;

  // TVector3 crystal_pos(1,0,0);
  // crystal_pos.SetZ(crystal_z);
  // crystal_pos.SetPhi(crystal_phi);
  // crystal_pos.SetPerp(perp_distance);

  // TVector3 global_pos = CrystalToGlobal(detnum, crystal_pos);


  // return global_pos;
}

void TCAGRA::LoadDetectorPositions() {
  static bool loaded = false;
  if(loaded){
    return;
  }
  loaded = true;

  //std::string filename = std::string(getenv("GRUTSYS")) + "/../config/SeGA_rotations.txt";
  std::string filename = std::string(getenv("GRUTSYS")) + "/config/CAGRA_positions.txt";

  //Read the locations from file.
  std::ifstream infile(filename);

  if(!infile){
    std::cout << "CAGRA positions file \"" << filename << "\""
              << " does not exist, skipping" << std::endl;
    return;
  }

  std::string line;
  while (std::getline(infile,line)) {
    //Parse the line
    int nclover, nsegment;
    char ncrystal;
    double x,y,z;
    int extracted = sscanf(line.c_str(),"clover.%02d.%c.%01d.vec: %lf %lf %lf",
                           &nclover,&ncrystal,&nsegment,&x,&y,&z);
    if (extracted!=6) {
      continue;
    }

    int index = (nclover << 16) + (((int)ncrystal) << 8) + nsegment;


    //Pack into the vector of transformations.
    TVector3 vec = TVector3(x,y,z);
    detector_positions[index] = vec;
  }
}

void TCAGRA::Print(Option_t *opt) const { }

void TCAGRA::Clear(Option_t *opt) {
  cagra_hits.clear();
}
