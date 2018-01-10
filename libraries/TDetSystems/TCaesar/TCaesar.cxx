#include "TCaesar.h"

#include <deque>

#include "TNSCLEvent.h"

#define FERA_TIME_ID        0x2301
#define FERA_ENERGY_ID      0x2302
#define FERA_TIMESTAMP_ID   0x2303
#define FERA_ERROR_ID       0x23ff

/*
 *  INPUT:  Two TCaesarHit objects
 * OUTPUT:  True if the two TCaesarHit object are within the given distance and time window
 *PURPOSE:  This function determines whether two TCaesarHit variables can be added together
 *          to form a single Caesar Addback hit based on whether they come within a certain
 *          time window and they are in neighboring detectors
 *.
 */
bool DefaultAddback(const TCaesarHit& one,const TCaesarHit &two){
  int ring = one.GetRingNumber();
  int det  = one.GetDetectorNumber();

  int poss_neigh_ring = two.GetRingNumber();
  int poss_neigh_det  = two.GetDetectorNumber();
  int num_neighbors_total = TCaesar::num_neighbors[ring][det];

  
  if (TMath::Abs(one.GetTime()-two.GetTime()) > 100){
    return false;
  }
  for (int neigh = 0; neigh < num_neighbors_total; neigh++){
    int neigh_ring = TCaesar::neighbors[ring][det][neigh][0];
    int neigh_det  = TCaesar::neighbors[ring][det][neigh][1];

    if (poss_neigh_ring == neigh_ring && poss_neigh_det == neigh_det){
      return true;
    }
  }//loop over possible neighbors
  return false;
}

std::function<bool(const TCaesarHit&,const TCaesarHit&)> TCaesar::fAddbackCondition = DefaultAddback;

void TCaesar::BuildAddback() const {
  if( addback_hits.size() > 0 ||
      caesar_hits.size() == 0) {
    return;
  }

  std::deque<const TCaesarHit*> hits;
  for(auto& hit : caesar_hits) {
    if (hit.IsValid()){
      hits.push_back(&hit);
    }
  }
  std::sort(hits.begin(), hits.end(), [](const TCaesarHit* a, const TCaesarHit* b) {
      TChannel *ca = TChannel::GetChannel(a->Address());
      TChannel *cb = TChannel::GetChannel(b->Address());
      if(!ca && !cb) {
        return a->Address()<b->Address();
      }
      if(!ca) {
        return true;
      }
      if(!cb) {
        return false;
      }

      return (ca->CalEnergy(static_cast<double>(a->Charge())) > cb->CalEnergy(static_cast<double>(b->Charge())));
  });

  std::vector<int> neighbor_positions;
  while(hits.size()) {

    addback_hits.push_back(*hits.front());
    hits.pop_front();
    TCaesarHit& new_hit = addback_hits.back();
    neighbor_positions.clear();
    for(int i=hits.size()-1; i>=0; i--) {
      const TCaesarHit& other_hit = *hits[i];
      //Need to now determine how many times this condition is satisfied for the
      //hit before adding them because this can cause issues where we add things that
      //should not be added
      if(fAddbackCondition(new_hit, other_hit)) {
        neighbor_positions.push_back(i);
      }
    }//loop over hits to possibly addback
    //Now do a switch based on number of neighbors!
    switch(neighbor_positions.size()){
      //No neighbors!
      case 0:
        continue;//nothing to do here, no neighbors so n0 event
      case 1:
        new_hit.AddToSelf(*hits[neighbor_positions.at(0)]);
        hits.erase(hits.begin()+neighbor_positions.at(0));
        break;
      case 2:
        //This is where things get hairy.
        //Need to ensure all three hits are neighbors if I'm going to add them back!
        if (fAddbackCondition(*hits[neighbor_positions.at(0)], *hits[neighbor_positions.at(1)])){
          new_hit.AddToSelf(*hits[neighbor_positions.at(0)]);
          new_hit.AddToSelf(*hits[neighbor_positions.at(1)]);
          hits.erase(hits.begin()+neighbor_positions.at(0));
          hits.erase(hits.begin()+neighbor_positions.at(1));
        }
        else{//garbage event! should set a flag
          new_hit.AddToSelf(*hits[neighbor_positions.at(0)]);
          new_hit.AddToSelf(*hits[neighbor_positions.at(1)]);
          hits.erase(hits.begin()+neighbor_positions.at(0));
          hits.erase(hits.begin()+neighbor_positions.at(1));
          new_hit.IsGarbageAddback();
        }
        break;
      default:
        for (unsigned int i = 0; i < neighbor_positions.size(); i++){
          new_hit.AddToSelf(*hits[neighbor_positions.at(i)]);
          hits.erase(hits.begin()+neighbor_positions.at(i));
        }
        new_hit.IsGarbageAddback();
        break;
    }//switch over number of neighbors
  }//while there are still hits in event
}

int  const TCaesar::det_per_ring[] = {10,14,24,24,24, 24, 24, 24, 14, 10};
char const TCaesar::ring_names[] = {'a','b','c','d','e','f','g','h', 'i','j'};

int TCaesar::vsnchn_ring_map_energy[MAX_VSN][MAX_CHN] = {{0}};
int TCaesar::vsnchn_ring_map_time[MAX_VSN][MAX_CHN] = {{0}};
int TCaesar::vsnchn_det_map_energy[MAX_VSN][MAX_CHN] = {{0}};
int TCaesar::vsnchn_det_map_time[MAX_VSN][MAX_CHN] = {{0}};
double TCaesar::detector_positions[N_RINGS][MAX_DETS][3] = {{{0.0,0.0,0.0}}};

int TCaesar::num_neighbors[N_RINGS][MAX_DETS] = {{0}};
int TCaesar::neighbors[N_RINGS][MAX_DETS][MAX_NEIGHBORS][2] = {{{{0}}}};
bool TCaesar::filled_map = false;
bool TCaesar::filled_det_pos = false;
bool TCaesar::filled_neighbor_map = false;
TCaesar::TCaesar() {
  if (!filled_map){
    ReadVSNMap(vsn_file_name);
    filled_map = true;
  }
  if (!filled_det_pos){
    ReadDetectorPositions(det_pos_file_name);
    filled_det_pos = true;
  }
  if (!filled_neighbor_map){
    ReadNeighborMap(neighbor_file_name);
    filled_neighbor_map = true;
  }
  Clear();
}



TCaesar::~TCaesar(){ }

void TCaesar::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TCaesar& caesar = (TCaesar&)obj;
  caesar.caesar_hits = caesar_hits;
}

void TCaesar::Clear(Option_t* opt){
  TDetector::Clear(opt);

  fUlm = -1;
  caesar_hits.clear();
  addback_hits.clear();
}

int TCaesar::BuildHits(std::vector<TRawEvent>& raw_data){

  for(auto& event : raw_data){
    TNSCLEvent& nscl = (TNSCLEvent&)event;
    SetTimestamp(nscl.GetTimestamp());
    Build_Single_Read(nscl.GetPayloadBuffer());
  }

  return caesar_hits.size();
}

TCaesarHit& TCaesar::GetCaesarHit(int i){
  return caesar_hits.at(i);
}

const TCaesarHit& TCaesar::GetCaesarHit(int i) const{
  return caesar_hits.at(i);
}

TDetectorHit& TCaesar::GetHit(int i){
  return caesar_hits.at(i);
}

void TCaesar::Print(Option_t *opt) const {
  printf("Caesar event @ %lu\n",Timestamp());
  printf("Number of Hits: %i\n",Size());
  for(unsigned int i=0;i<Size();i++) {
    printf("\t");
    GetCaesarHit(i).Print();
  }
  printf("---------------------------------------\n");
}
void TCaesar::PrintAddback(Option_t *opt)  {
  printf("Caesar addback event @ %lu\n",Timestamp());
  printf("Number of Addback Hits: %i\n",AddbackSize());
  for(int i=0;i<AddbackSize();i++) {
    printf("\t");
    GetAddbackHit(i).Print();
  }
  printf("---------------------------------------\n");
}

double TCaesar::GetCorrTime(TCaesarHit hit, TS800 *s800){
  if (!s800 || !hit.IsValid()){
    return sqrt(-1);
  }
  double caesar_time = hit.GetTime();
  double tac_obj = s800->GetTof().GetTacOBJ();
  double s800_source_time = s800->GetTrigger().GetS800Source();
//  return (caesar_time - ((s800source + tac_obj)*(0.1/0.25)));
  return GetCorrTime(caesar_time, s800_source_time, tac_obj);
}

double TCaesar::GetCorrTime(double caesar_time, double s800_source_time,
                            double scint_time){
  //CAESAR Timing 250 ps/channel
  //S800   Timing 100 ps/channel
  return (caesar_time - ((s800_source_time + scint_time)*(0.1/0.25)));
}

void TCaesar::Build_Single_Read(TSmartBuffer buf){
  const char* data = buf.GetData();
  const char* data_end = data + buf.GetSize();

  TRawEvent::CAESARHeader* header = (TRawEvent::CAESARHeader*)data;
  data += sizeof(TRawEvent::CAESARHeader);

  // FERA_VERSION_00 does not have a ULM register
  if(header->version == 0x0001){
    data -= sizeof(Short_t);
    SetULM(-1);
  } else {
    SetULM(header->ulm);
  }

  while(data < data_end){
    //fera_header contains the size & tag of the current packet
    TRawEvent::CAESARFeraHeader* fera_header = (TRawEvent::CAESARFeraHeader*)data;


    if((fera_header->tag != FERA_ENERGY_ID) && (fera_header->tag != FERA_TIME_ID)){
      data += fera_header->size * 2; // Size is inclusive number of 16-bit values.
      if(fera_header->tag == FERA_TIMESTAMP_ID){
        continue;
      }
      else if (fera_header->tag == FERA_ERROR_ID){
        break; // what we actually need to do is check if the fera is bad and take some action.
      }
      else {
        std::cout << "Unknown fera pkt tag" << (std::hex) << fera_header->tag << std::endl;
        break;
      }
    }
    const char* fera_end = data + fera_header->size*2;
    data += sizeof(TRawEvent::CAESARFeraHeader);

    while(data < fera_end){
      //This should contain all the data until the end of the fera
      TRawEvent::CAESARFera* fera = (TRawEvent::CAESARFera*)data;
      data += sizeof(TRawEvent::CAESARFera);
      int nchan = fera->number_chans();
      if(nchan==0){
	nchan = 16;
      }

      //Now we have the first header and know the VSN and number of channels
      //We need to start grabbing CAESARFeraItems now
      for(int i=0; i<nchan; i++){
        TRawEvent::CAESARFeraItem *item = (TRawEvent::CAESARFeraItem*)data;

	if(fera_header->tag == FERA_ENERGY_ID){
	  SetCharge(fera->vsn(), item->channel(), item->value());
	} else { //FERA_TIME_ID
	  SetTime(fera->vsn(), item->channel(), item->value());
	}
        data += sizeof(TRawEvent::CAESARFeraItem);//just read in a single CAESARFeraItem
      }
    }
  }
}

TCaesarHit& TCaesar::GetHit_VSNChannel(int vsn, int channel){
  for(auto& hit : caesar_hits){
    if(hit.GetVSN() == vsn &&
       hit.GetChannel() == channel){
      return hit;
    }
  }

  // No such thing existing right now, make one
  caesar_hits.emplace_back();
  TCaesarHit& output = caesar_hits.back();
  output.SetVSN(vsn);
  output.SetChannel(channel);
  output.SetAddress( (37<<24) +
		     (vsn<<16) +
		     (channel) );
  output.SetDet(vsnchn_det_map_energy[vsn][channel] );
  output.SetRing(vsnchn_ring_map_energy[vsn][channel]);
  return output;
}


void TCaesar::SetCharge(int vsn, int channel, int data) {
  TCaesarHit& hit = GetHit_VSNChannel(vsn, channel);
  hit.SetCharge(data);
}

void TCaesar::SetTime(int vsn, int channel, int data) {
  TCaesarHit& hit = GetHit_VSNChannel(vsn, channel);
  hit.SetTime(data);
}

void TCaesar::InsertHit(const TDetectorHit& hit) {
  caesar_hits.emplace_back((TCaesarHit&)hit);
  fSize++;
}

void TCaesar::ReadDetectorPositions(std::string in_file_name){
  std::cout << "Mapping detectors <-> Positions"<< std::endl;
  std::ifstream input_file;
  input_file.open(in_file_name.c_str());
  if (!input_file){
    std::cout << "Detector input file: " << in_file_name <<  " doesn't exist!" << std::endl;
    return;
  }
  std::string line;

  char ring_name;
  char pos_name;
  int det_id;
  int ring_id;
  int pos_id;
  double pos;

  while(std::getline(input_file,line)){
    sscanf(line.c_str(), "Caesar.Ring.%c.pos_%c.%d:\t %lf", &ring_name, &pos_name, &det_id, &pos);
    det_id = det_id-1; //want det_id to start from 0
    ring_id = ring_name - 'a';//forces ring_id to start from 0
    pos_id = pos_name - 'x';//want pos_name to start from 0 at x
    this->detector_positions[ring_id][det_id][pos_id] = pos;
  }
}

void TCaesar::ReadNeighborMap(std::string in_file_name){
  TEnv *map = new TEnv(in_file_name.c_str());
  for (int ring = 0; ring < N_RINGS; ring++){
    for (int det = 0; det < det_per_ring[ring]; det++){
      num_neighbors[ring][det] = map->GetValue(Form("Caesar.Neigh.Ring.%c.%d", ring_names[ring],det+1),0);
      for (int neigh = 0; neigh < num_neighbors[ring][det]; neigh++){
        //-1 because we want to count from 0
	neighbors[ring][det][neigh][0] = map->GetValue(Form("Caesar.Neigh.Ring.%c.Det.%d.Ring.%d",ring_names[ring],det+1,neigh),0) - 1;
	neighbors[ring][det][neigh][1] = map->GetValue(Form("Caesar.Neigh.Ring.%c.Det.%d.Det.%d", ring_names[ring],det+1,neigh),0) - 1;
      }
    }
  }
}

void TCaesar::ReadVSNMap(std::string in_file_name){
  //Note that in the VSN mapping file, VSN is referenced from 1 while the channel
  //is referenced from 0.

  std::cout << "Mapping detectors <-> FERA modules"<< std::endl;

  for(int vsn=0; vsn<MAX_VSN; vsn++){
    for(int chn=0; chn<MAX_CHN; chn++){
      vsnchn_ring_map_energy[vsn][chn] = -1;
      vsnchn_det_map_energy [vsn][chn] = -1;
      vsnchn_ring_map_time[vsn][chn] = -1;
      vsnchn_det_map_time [vsn][chn] = -1;
    }
  }

  TEnv *map = new TEnv(in_file_name.c_str());
  for(int ring=0; ring < N_RINGS; ring++){
    for(int det=1; det <= det_per_ring[ring]; det++){

      int vsn_e = map->GetValue(Form("Fera.Ring.%c.vsn_en.%d",ring_names[ring],det),-1);
      int vsn_t = map->GetValue(Form("Fera.Ring.%c.vsn_ti.%d",ring_names[ring],det),-1);
      int chn_e = map->GetValue(Form("Fera.Ring.%c.chn_en.%d",ring_names[ring],det),-1);
      int chn_t = map->GetValue(Form("Fera.Ring.%c.chn_ti.%d",ring_names[ring],det),-1);


      if((vsn_e != vsn_t) || (chn_e != chn_t))
        std::cout << "  >>>>> WARNING: ring " << ring_names[ring] << " det "  << det << " has not equal mapping for time AND energy" << std::endl;

      vsn_e--;//forces vsn_e to be referennced from 0
      if(vsn_e>-1 && chn_e>-1){
        if(vsn_e < MAX_VSN &&  chn_e< MAX_CHN){
          if(vsnchn_ring_map_energy[vsn_e][chn_e] == -1 || vsnchn_det_map_energy [vsn_e][chn_e] == -1){ // was not yet set
            vsnchn_ring_map_energy[vsn_e][chn_e] = ring;
            vsnchn_det_map_energy [vsn_e][chn_e] = det-1;//det runs now from 0
          }
          else{
            std::cout << "  >>>>> Error of mapping detectors to VSN/CHN for energy!!!" << std::endl
                 << "Ring " << ring_names[ring] << " "
                 << "Det " << det << " "
                 << "is assigned to VSN " << vsn_e+1 << " Chn " << chn_e
                 << std::endl
                 << "but this combination is already occupied by "
                 << ring_names[vsnchn_ring_map_energy[vsn_e][chn_e]]
                 << " Det " << vsnchn_det_map_energy [vsn_e][chn_e]
                 << std::endl
                 << "  >>>>> THIS DETECTOR IS OMITTED"
                 << std::endl;
          }
        }// < max vsn/ch
        else{
          std::cout << "  >>>>> Error of mapping detectors to VSN/CHN for energy!!!" << std::endl
               << "Ring " << ring_names[ring]
               << "Det " << det
               << "is assigned to VSN " << vsn_e+1 << " Chn " << chn_e
               << " exceeds limits VSN/chn of " << MAX_VSN << "/"
               << MAX_CHN << std::endl;

        }
      }//read a value

     vsn_t--;
      if(vsn_t>-1 && chn_t>-1){
        if(vsn_t < MAX_VSN &&  chn_t< MAX_CHN){
          if(vsnchn_ring_map_time[vsn_t][chn_t] == -1 || vsnchn_det_map_time [vsn_t][chn_t] == -1){ // was not yet set
            vsnchn_ring_map_time[vsn_t][chn_t] = ring;
            vsnchn_det_map_time [vsn_t][chn_t] = det-1;//det runs now from 0
          }
          else{
            std::cout << "  >>>>> Error of mapping detectors to VSN/CHN for energy!!!" << std::endl
                 << "Ring " << ring_names[ring] << " "
                 << "Det " << det << " "
                 << "is assigned to VSN " << vsn_t+1 << " Chn " << chn_t
                 << std::endl
                 << "but this combination is already occupied by "
                 << ring_names[vsnchn_ring_map_time[vsn_t][chn_t]]
                 << " Det " << vsnchn_det_map_time [vsn_t][chn_t]
                 << std::endl
                 << "  >>>>> THIS DETECTOR IS OMITTED"
                 << std::endl;
          }
        }// < max vsn/ch
        else{
          std::cout << "  >>>>> Error of mapping detectors to VSN/CHN for energy!!!" << std::endl
               << "Ring " << ring_names[ring]
               << "Det " << det
               << "is assigned to VSN " << vsn_t+1 << " Chn " << chn_t
               << " exceeds limits VSN/chn of " << MAX_VSN << "/"
               << MAX_CHN << std::endl;

        }
      }//read a value

    }//det
  }//rings
  return;
}

TVector3 TCaesar::GetPosition(int ring,int det) {
  double x = detector_positions[ring][det][0];
  double y = detector_positions[ring][det][1];
  double z = detector_positions[ring][det][2];

  double shift = GValue::Value("TARGET_SHIFT_X");
  if(!std::isnan(shift)) {
    x -= shift;
  }

  shift = GValue::Value("TARGET_SHIFT_Y");
  if(!std::isnan(shift)) {
    y -= shift;
  }

  shift = GValue::Value("TARGET_SHIFT_Z");
  if(!std::isnan(shift)) {
    z -= shift;
  }

  //Positions in file are in cm; need to be in mm
  x*= 10.0;
  y*= 10.0;
  z*= 10.0;

  return TVector3(x,y,z);
}

//Note: z_shift must be in centimeters
TVector3 TCaesar::GetPosition(int ring,int det, double z_shift) {
  double x = detector_positions[ring][det][0];
  double y = detector_positions[ring][det][1];
  double z = detector_positions[ring][det][2];

  z -= z_shift;

  //Positions in file are in cm; need to be in mm
  x*= 10.0;
  y*= 10.0;
  z*= 10.0;

  return TVector3(x,y,z);
}
