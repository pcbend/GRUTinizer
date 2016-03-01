#include "TSega.h"

#include <iostream>
#include <fstream>

#include "DDASDataFormat.h"
#include "TNSCLEvent.h"
#include "TChannel.h"

std::map<int,TSega::Transformation> TSega::detector_positions;

TSega::TSega(){ }

TSega::~TSega(){ }

void TSega::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TSega& sega = (TSega&)obj;
  sega.sega_hits = sega_hits;
  sega.raw_data.clear();
}

void TSega::Clear(Option_t* opt){
  TDetector::Clear(opt);

  sega_hits.clear();
}

void TSega::Draw(Option_t *opt) {
  //if(!gPad || !gPad->IsEditable())  {
  //  gROOT->MakeDefCanvas();
  //} else {
  //  gPad->GetCanvas()->Clear();
  //}





}

TSegaHit& TSega::GetSegaHit(int i){
  return sega_hits.at(i);
}

TDetectorHit& TSega::GetHit(int i){
  return sega_hits.at(i);
}

int TSega::BuildHits() {
  for(auto& event : raw_data){
    TNSCLEvent& nscl = (TNSCLEvent&)event;

    SetTimestamp(nscl.GetTimestamp());

    TDDASEvent ddas(nscl.GetPayloadBuffer());

    unsigned int address = ( (1<<24) +
                             (ddas.GetCrateID()<<16) +
                             (ddas.GetSlotID()<<8) +
                             ddas.GetChannelID() );
    TChannel* chan = TChannel::GetChannel(address);

    static int lines_displayed = 0;
    if(!chan){
      if(lines_displayed < 10) {
        std::cout << "Unknown (crate, slot, channel): ("
                  << ddas.GetCrateID() << ", " << ddas.GetSlotID()
                  << ", " << ddas.GetChannelID() << ")" << std::endl;
      }//  else if(lines_displayed==1000){
      //   std::cout << "I'm going to stop telling you that the channel was unknown,"
      //             << " you should probably stop the program." << std::endl;
      // }
      lines_displayed++;

      // sega_hits.emplace_back();
      // TSegaHit* hit = &sega_hits.back();
      // hit->SetAddress(address);
      // hit->SetTimestamp(nscl.GetTimestamp());
      continue;
    }

    int detnum = chan->GetArrayPosition();
    int segnum = chan->GetSegment();

    // Get a hit, make it if it does not exist
    TSegaHit* hit = NULL;
    for(auto& ihit : sega_hits){
      if(ihit.GetDetnum() == detnum){
        hit = &ihit;
        break;
      }
    }
    if(hit == NULL){
      sega_hits.emplace_back();
      hit = &sega_hits.back();
      fSize++;
    }

    if(segnum==0){
      hit->SetAddress(address);
      hit->SetTimestamp(nscl.GetTimestamp());
      hit->SetCharge(ddas.GetEnergy());
      hit->SetTrace(ddas.GetTraceLength(), ddas.trace);
    } else {
      TSegaSegmentHit& seg = hit->MakeSegmentByAddress(address);
      seg.SetCharge(ddas.GetEnergy());
      seg.SetTimestamp(ddas.GetTimestamp());
      seg.SetTrace(ddas.GetTraceLength(), ddas.trace);
    }
  }

  return Size();
}

TVector3 TSega::GetSegmentPosition(int detnum, int segnum) {
  if(detnum < 0 || detnum > 15 ||
     segnum < 1 || segnum > 32){
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

TVector3 TSega::CrystalToGlobal(int detnum, TVector3 crystal_pos) {
  LoadDetectorPositions();

  static int lines_displayed = 0;
  if(!detector_positions.count(detnum)){
    if(lines_displayed < 1000) {
      std::cout << "No transformation matrix loaded for SeGA det " << detnum << std::endl;
    } else if (lines_displayed == 1000){
      std::cout << "I'm going to stop telling you about the missing matrix,"
                << " you should probably stop the program." << std::endl;
    }
    lines_displayed++;
    return TVector3(std::sqrt(-1),std::sqrt(-1),std::sqrt(-1));
  }

  Transformation& trans = detector_positions[detnum];
  return trans.origin + crystal_pos.X()*trans.x + crystal_pos.Y()*trans.y + crystal_pos.Z()*trans.z;
}

void TSega::LoadDetectorPositions() {
  static bool loaded = false;
  if(loaded){
    return;
  }
  loaded = true;

  //std::string filename = std::string(getenv("GRUTSYS")) + "/../config/SeGA_rotations.txt";
  std::string filename = std::string(getenv("GRUTSYS")) + "/config/SeGA_rotations.txt";

  //Read the locations from file.
  std::ifstream infile(filename);

  if(!infile){
    std::cout << "SeGA rotation matrix file \"" << filename << "\""
             << " does not exist, skipping" << std::endl;
    return;
  }

  std::string line;
  while (std::getline(infile,line)) {
    //Parse the line
    int detnum;
    char name_c[20];
    double x,y,z;
    int extracted = sscanf(line.c_str(),"det.%02d.%6s: %lf %lf %lf",
                           &detnum,name_c,&x,&y,&z);
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

void TSega::InsertHit(const TDetectorHit& hit) {
  sega_hits.emplace_back((TSegaHit&)hit);
  fSize++;
}
