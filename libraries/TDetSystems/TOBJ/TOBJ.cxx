
#include <TMath.h>

#include <TOBJ.h>

#include <TRawEvent.h>
#include <DDASDataFormat.h>
#include <DDASBanks.h>

ClassImp(TOBJ)


int TOBJ::BuildHits(std::vector<TRawEvent>& raw_data) {
  for(auto& event : raw_data){
    SetTimestamp(event.GetTimestamp());
    TSmartBuffer buf = event.GetPayloadBuffer();
    int total_size = *(int*)buf.GetData();
    const char* buffer_end = buf.GetData() + total_size;
    // uncomment below two lines if we are getting data from gebpush
    //buf.Advance(sizeof(int));
    //buf.Advance(sizeof(short)); // i am now even more confused.  extra 0x0000 after first ncsl ts
    //buf.Advance(sizeof(int));  // i dont know why this is???  2019-Pt run pcb. 
    //int ptr = sizeof(int);
    while(buf.GetData() < buffer_end) {
      // Constructor advances the buffer to end of each channel
      //TDDASEvent<DDASGEBHeader> ddas(buf);
      TDDASEvent<DDASHeader> ddas(buf);
      //std::cout << ddas.GetTimestamp() << "     ddas.GetAddress():   " << ddas.GetAddress() << std::endl;
      
      /////////////////
      /////////////////
      /////////////////
      TOBJHit hit;
      hit.SetAddress(ddas.GetAddress());
      hit.SetCharge(ddas.GetEnergy());
      hit.SetTime(ddas.GetCFDTime());
      hit.SetTimestamp(ddas.GetTimestamp());
      hit.SetTrace(ddas.GetTraceLength(),ddas.trace);
      hit.SetPileup(ddas.GetFinishCode());
//      hit.TrigFilter(0.192,0.192);
//      hit.EnergyFilter(0.192,0.048,0.050);
//      if(ddas.energy_sum!=NULL)  std::cout<<"energy_sum: "<<ddas.energy_sum->energy_sum[0]<<std::endl;;
      hit.SetExternalTimestamp(ddas.GetExternalTimestamp());
      //InsertHit(hit);
      obj_hits.push_back(hit);
      /////////////////
      /////////////////
      /////////////////
      
      int ddas_card    = ddas.GetSlotID(); 
      int ddas_channel = ddas.GetChannelID();
      //int ddas_energy  = ddas.GetEnergy();
      //unsigned int ddas_address = ddas.GetAddress();
    /*
      std::cout << "address\t0x" << std::hex << ddas_address << std::dec << std::endl; 
      std::cout << "chan   \t" <<  ddas_channel << std::endl; 
      std::cout << "card   \t" <<  ddas_card    << std::endl; 
      std::cout << "eng    \t" <<  ddas_energy << std::endl; 
      std::cout << "--------------------- "  << std::endl;
     */ 
     
        if(ddas_card==2) { // 
          switch(ddas_channel) {
            case 0:     //pin1
//              fobj.SetAddress(ddas.GetAddress());
//              fobj.SetCharge(ddas.GetEnergy());
//              fobj.SetTime(ddas.GetCFDTime());
//              fobj.SetTimestamp(ddas.GetTimestamp());
//              fPin1.SetExternalTimestamp(ddas.GetExternalTimestamp());
              break;
            case 1:     //pin2
//              fPin2.SetExternalTimestamp(ddas.GetExternalTimestamp());
              break;
            case 2:     //implant
              break;
            case 3:     //veto
              break;
            case 4:     //xfp cfd
              break;
            case 5:     //xfp analog
              break;
            case 6:     //tac1  pin1-xfp
              break;
            case 7:     //tac2  pin2-xfp
              break; 
            case 8:     //tac3  sssd-xfp
              break;
            case 9:     //tac4  implant-xfp
              break;
            case 10:    //tac5  pin1-rf
              break;
            case 11:    //"pin4 high-gain"  -> veto high gain
              break;
            case 12:    //xtal q6p4
              break;
            case 13:    //
              break;
            case 14:    // 
              break;
            case 15:    // low hertz pulser
              break;
          };
         

        } else if(ddas_card==3) { //sssd detector
        }

       
    }
  }
  fSize = obj_hits.size();
  return fSize;
}
