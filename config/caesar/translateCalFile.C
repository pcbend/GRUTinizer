#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include "TEnv.h"


#define N_RINGS 10
#define MAX_DETS 24
#define MAX_CHN 16
#define MAX_VSN 12

void ReadVSNMap(std::string in_file_name, int vsnchn_ring_map_energy[MAX_VSN][MAX_CHN], 
                                          int vsnchn_det_map_energy[MAX_VSN][MAX_CHN], 
                                          int vsnchn_ring_map_time[MAX_VSN][MAX_CHN], 
                                          int vsnchn_det_map_time[MAX_VSN][MAX_CHN]){
  //Note that in the VSN mapping file, VSN is referenced from 1 while the channel
  //is referenced from 0.
  
  int  const det_per_ring[] = {10,14,24,24,24, 24, 24, 24, 14, 10};
  char const ring_names[] = {'a','b','c','d','e','f','g','h', 'i','j'};
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

void translateCalFile(std::string vsn_map_file_name, std::string ecal_file_name,
                      std::string tcal_file_name, std::string out_file_name){

  int  const det_per_ring[] = {10,14,24,24,24, 24, 24, 24, 14, 10};
  char const ring_names[] = {'a','b','c','d','e','f','g','h', 'i','j'};

  double e_cal_par[N_RINGS][MAX_DETS][3];
  double t_cal_par[N_RINGS][MAX_DETS];//just a constant offset
  for (int i = 0; i < N_RINGS; i++){
    for (int j = 0; j  < MAX_DETS; j++){
      e_cal_par[i][j][0] = 0.0;
      e_cal_par[i][j][1] = 0.0;
      e_cal_par[i][j][2] = 0.0;
      t_cal_par[i][j] = 0.0;
    }
  }

  //First parse the GrROOT calibration file to fill the cal_par array and
  //get the VSN map so I can relate VSN/CHN numbers to DET/RING values
  //(necessary for addressing)
  int vsnchn_ring_map_energy[MAX_VSN][MAX_CHN];
  int vsnchn_det_map_energy[MAX_VSN][MAX_CHN];
  int vsnchn_ring_map_time[MAX_VSN][MAX_CHN];
  int vsnchn_det_map_time[MAX_VSN][MAX_CHN];
  ReadVSNMap(vsn_map_file_name, vsnchn_ring_map_energy, vsnchn_det_map_energy,
                                vsnchn_ring_map_time,   vsnchn_det_map_time);



  std::ofstream out_chan_file; //File in correct format for TChannel
  out_chan_file.open(out_file_name.c_str());

  TEnv *e_cal_file = new TEnv(ecal_file_name.c_str());
  TEnv *t_cal_file = new TEnv(tcal_file_name.c_str());
  for (int ring = 0; ring < N_RINGS; ring++){
    for (int det = 1; det <= det_per_ring[ring]; det++){
      e_cal_par[ring][det-1][0] = e_cal_file->GetValue(Form("Caesar.Ring.%c.e_a0.%d", 
                                                        ring_names[ring],det),-1.0);
      e_cal_par[ring][det-1][1] = e_cal_file->GetValue(Form("Caesar.Ring.%c.e_a1.%d", 
                                                        ring_names[ring],det),-1.0);
      e_cal_par[ring][det-1][2] = e_cal_file->GetValue(Form("Caesar.Ring.%c.e_a2.%d", 
                                                        ring_names[ring],det),-1.0);
      t_cal_par[ring][det-1] = t_cal_file->GetValue(Form("Caesar.Ring.%c.t_a0.%d", 
                                                       ring_names[ring],det),-1.0);
    }
  }

  //So now I need to create a new file with the following structure:
  //RING00DET00 {
  //  Name : 
  //  Address : 
  //  ENERGYCOEFF : 
  //  TIMECOEFF:
  //}
  for (int vsn = 0; vsn < MAX_VSN; vsn++){
    for (int chan = 0; chan < MAX_CHN; chan++){
      int ring = vsnchn_ring_map_energy[vsn][chan];
      int det  = vsnchn_det_map_energy[vsn][chan];


     
      out_chan_file.fill('0');
      out_chan_file << "RING" << setw(2) << ring << "DET" << setw(2) << det << " {" << std::endl
                    << "  Address: 0x" << (std::hex) << ((37 << 24) + (vsn << 16) + chan) << std::dec << std::endl
                    << "  ENERGYCOEFF: " << e_cal_par[ring][det][0] << " " 
                                         << e_cal_par[ring][det][1] << " " 
                                         << e_cal_par[ring][det][2] << std::endl
                    << "  TIMECOEFF:  "  << t_cal_par[ring][det]     
                                         << "  1" << std::endl
                    << "}" << std::endl;
    }
  }

  out_chan_file.close();
}
