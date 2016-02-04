#include "TCaesar.h"

#include "TNSCLEvent.h"

#define FERA_TIME_ID        0x2301
#define FERA_ENERGY_ID      0x2302
#define FERA_TIMESTAMP_ID   0x2303
#define FERA_ERROR_ID       0x23ff

#define DEBUG_BRANDON 0


int  const TCaesar::det_per_ring[] = {10,14,24,24,24, 24, 24, 24, 14, 10};
char const TCaesar::ring_names[] = {'a','b','c','d','e','f','g','h', 'i','j'};

int TCaesar::vsnchn_ring_map_energy[MAX_VSN][MAX_CHN] = {{0}};
int TCaesar::vsnchn_ring_map_time[MAX_VSN][MAX_CHN] = {{0}};
int TCaesar::vsnchn_det_map_energy[MAX_VSN][MAX_CHN] = {{0}};
int TCaesar::vsnchn_det_map_time[MAX_VSN][MAX_CHN] = {{0}};
double TCaesar::detector_positions[N_RINGS][MAX_DETS][3] = {{{0.0,0.0,0.0}}};

bool TCaesar::filled_map = false;
bool TCaesar::filled_det_pos = false;
TCaesar::TCaesar() {
  if (!filled_map){
    ReadVSNMap(vsn_file_name);
    filled_map = true;
  }
  if (!filled_det_pos){
    ReadDetectorPositions(det_pos_file_name);
    filled_det_pos = true;
  }
  Clear();
}



TCaesar::~TCaesar(){ }

void TCaesar::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TCaesar& caesar = (TCaesar&)obj;
  caesar.caesar_hits = caesar_hits;
  caesar.raw_data.clear();
}

void TCaesar::Clear(Option_t* opt){
  TDetector::Clear(opt);

  fUlm = -1;
  caesar_hits.clear();
}

int TCaesar::BuildHits(){
  //if(raw_data.size()!=1) 
    //printf("\nCaesar event size = %i\n",raw_data.size());

  for(auto& event : raw_data){
    TNSCLEvent& nscl = (TNSCLEvent&)event;
    SetTimestamp(nscl.GetTimestamp());
    Build_Single_Read(nscl.GetPayloadBuffer());
//  Print();
//  event.Print("all");
//  printf("********************************\n");
//  printf("********************************\n");
//  printf("********************************\n");
//  printf("********************************\n");
//  printf("********************************\n");
  }
  //if(caesar_hits.size()<1) {
  //  raw_data.at(0).Print("all"); 
  //  printf("\nCaesar hits size = %i\n",caesar_hits.size());
  //  Print();
  //}
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
  printf("Casear event @ %lu\n",Timestamp());
  printf("Number of Hits: %i\n",Size());
  for(int i=0;i<Size();i++) {
    printf("\t"); 
    GetCaesarHit(i).Print();
  }
  printf("---------------------------------------\n");
}

//Calculate E_cm = gamma *(1-beta*cos(angle))E_lab
double TCaesar::GetEnergyDC(TCaesarHit hit){
  double BETA = GValue::Value("BETA");
  double Z_SHIFT = GValue::Value("TARGET_SHIFT_Z");
  
  if (!BETA){
    std::cout << "No Beta given, can't correct" << std::endl;
    return sqrt(-1);
  }
  if (!Z_SHIFT){
    std::cout << "Warning no Z-shift applied" << std::endl;
  }
  int ring = hit.GetRingNumber();
  int det = hit.GetDetectorNumber();
  double x = detector_positions[ring][det][0];
  double y = detector_positions[ring][det][1];
  double z = detector_positions[ring][det][2];

  //cos_angle is equal to z/(z^2+x^2+y^2) where x,y,z have to be corrected for shift
  double cos_angle = (z-Z_SHIFT)/(sqrt(pow((z-Z_SHIFT),2)+x*x+y*y));
  double gamma = 1.0/(sqrt(1-BETA*BETA));

  return (gamma*(1-BETA*cos_angle)*hit.GetEnergy());
}

double TCaesar::GetCorrTime(TCaesarHit hit, TS800 *s800){
  double caesar_time = hit.GetTime();
  if (!s800){
    return sqrt(-1);
  }
  
  double tac_obj = s800->GetTof().GetTacOBJ();
  double s800source = s800->GetTrigger().GetS800Source();
  return caesar_time - ((s800source + tac_obj)*(0.1/0.25));
}
void TCaesar::Build_Single_Read(TSmartBuffer buf){
  const char* data = buf.GetData();
  const char* data_end = data + buf.GetSize();

  if (DEBUG_BRANDON){
    std::cout << "--------------------------------------------------\n\n" << std::endl;
    buf.Print("all");
  }
  

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

    if (DEBUG_BRANDON){
      std::cout << "fera_header->size = 0x" << (std::hex) << fera_header->size << std::endl;
      std::cout << "fera_header->tag = 0x" << (std::hex) << fera_header->tag << std::endl;
    }


    //printf("fera end:  0x%04x\n",*((unsigned short*)fera_end));
    //buf.Print("all");


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
      if (DEBUG_BRANDON){
        std::cout << "fera->header = 0x" << (std::hex) << fera->header << std::endl;
        std::cout << "fera->number_chans() = " << (std::dec) << fera->number_chans() << std::endl;
        std::cout << "fera->vsn() = " <<  (std::dec) << fera->vsn() << std::endl;
      }
      for(int i=0; i<nchan; i++){
        TRawEvent::CAESARFeraItem *item = (TRawEvent::CAESARFeraItem*)data;
        if (DEBUG_BRANDON){
          std::cout << "item->data = 0x"    << (std::hex)     << item->data    << std::endl;
          std::cout << "item->channel() = " << (std::dec) << item->channel() << std::endl;
          std::cout << "item->value() = "   << (std::dec) << item->value()   << std::endl;
        }

	if(fera_header->tag == FERA_ENERGY_ID){
	  SetCharge(fera->vsn(), item->channel(), item->value());
	} else { //FERA_TIME_ID
	  SetTime(fera->vsn(), item->channel(), item->value());
	}
        data += sizeof(TRawEvent::CAESARFeraItem);//just read in a single CAESARFeraItem
      }
      //data += 2*fera->number_chans() + 2;
    }
    if (DEBUG_BRANDON){
      std::cout << "--------------------------------------------------\n\n" << std::endl;
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
      //cout << Form("Fera.Ring.%c.chn_en.%d",ring_names[ring],det) << "\t";
      //cout << map->GetValue(Form("Fera.Ring.%c.chn_en.%d",ring_names[ring],det),-1) << endl;
      
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

