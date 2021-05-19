
#include <TMath.h>

#include <TUML.h>

#include <TRawEvent.h>
#include <DDASDataFormat.h>
#include <DDASBanks.h>

ClassImp(TUML)


int TUML::BuildHits(std::vector<TRawEvent>& raw_data) {
  for(auto& event : raw_data){
    SetTimestamp(event.GetTimestamp());
    TSmartBuffer buf = event.GetPayloadBuffer();
    int total_size = *(int*)buf.GetData();
    const char* buffer_end = buf.GetData() + total_size;
    buf.Advance(sizeof(int));
    buf.Advance(sizeof(short)); // i am now even more confused.  extra 0x0000 after first ncsl ts
    //buf.Advance(sizeof(int));  // i dont know why this is???  2019-Pt run pcb. 
    //int ptr = sizeof(int);
    while(buf.GetData() < buffer_end) {
      // Constructor advances the buffer to end of each channel
      TDDASEvent<DDASGEBHeader> ddas(buf);
      //std::cout << ddas.GetTimestamp() << "     ddas.GetAddress():   " << ddas.GetAddress() << std::endl;
      
      /////////////////
      /////////////////
      /////////////////
      TUMLHit hit;
      hit.SetAddress(ddas.GetAddress());
      hit.SetCharge(ddas.GetEnergy());
      hit.SetTime(ddas.GetCFDTime());
      hit.SetTimestamp(ddas.GetTimestamp());
      hit.SetExternalTimestamp(ddas.GetExternalTimestamp());
      //InsertHit(hit);
      uml_hits.push_back(hit);
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
              fPin1.SetAddress(ddas.GetAddress());
              fPin1.SetCharge(ddas.GetEnergy());
              fPin1.SetTime(ddas.GetCFDTime());
              fPin1.SetTimestamp(ddas.GetTimestamp());
              fPin1.SetExternalTimestamp(ddas.GetExternalTimestamp());
              break;
            case 1:     //pin2
              fPin2.SetAddress(ddas.GetAddress());
              fPin2.SetCharge(ddas.GetEnergy());
              fPin2.SetTime(ddas.GetCFDTime());
              fPin2.SetTimestamp(ddas.GetTimestamp());
              fPin2.SetExternalTimestamp(ddas.GetExternalTimestamp());
              break;
            case 2:     //implant
              fImplant.SetAddress(ddas.GetAddress());
              fImplant.SetCharge(ddas.GetEnergy());
              fImplant.SetTime(ddas.GetCFDTime());
              fImplant.SetTimestamp(ddas.GetTimestamp());
              fImplant.SetExternalTimestamp(ddas.GetExternalTimestamp());
              break;
            case 3:     //veto
              fVeto.SetAddress(ddas.GetAddress());
              fVeto.SetCharge(ddas.GetEnergy());
              fVeto.SetTime(ddas.GetCFDTime());
              fVeto.SetTimestamp(ddas.GetTimestamp());
              fVeto.SetExternalTimestamp(ddas.GetExternalTimestamp());
              break;
            case 4:     //xfp cfd
              fXfp1.SetAddress(ddas.GetAddress());
              fXfp1.SetCharge(ddas.GetEnergy());
              fXfp1.SetTime(ddas.GetCFDTime());
              fXfp1.SetTimestamp(ddas.GetTimestamp());
              fXfp1.SetExternalTimestamp(ddas.GetExternalTimestamp());
              break;
            case 5:     //xfp analog
              fXfp2.SetAddress(ddas.GetAddress());
              fXfp2.SetCharge(ddas.GetEnergy());
              fXfp2.SetTime(ddas.GetCFDTime());
              fXfp2.SetTimestamp(ddas.GetTimestamp());
              fXfp2.SetExternalTimestamp(ddas.GetExternalTimestamp());
              break;
            case 6:     //tac1  pin1-xfp
              fTac1 = ddas.GetEnergy();
              break;
            case 7:     //tac2  pin2-xfp
              fTac2 = ddas.GetEnergy();
              break; 
            case 8:     //tac3  sssd-xfp
              fTac3 = ddas.GetEnergy();
              break;
            case 9:     //tac4  implant-xfp
              fTac4 = ddas.GetEnergy();
              break;
            case 10:    //tac5  pin1-rf
              fTac5 = ddas.GetEnergy();
              break;
            case 11:    //"pin4 high-gain"  -> veto high gain
              break;
            case 12:    //xtal q6p4
              gamma_energy = ddas.GetEnergy(); //!
              gamma_time = ddas.GetTimestamp();   //!
              //std::cout << "ge:  " << gamma_energy << "\t";
              //std::cout << "gt:  " << gamma_time   << "\n";
              break;
            case 13:    //
              break;
            case 14:    // 
              break;
            case 15:    // low hertz pulser
              break;
          };
         

        } else if(ddas_card==3) { //sssd detector
          TUMLHit hit;
          hit.SetAddress(ddas.GetAddress());
          hit.SetCharge(ddas.GetEnergy());
          hit.SetTime(ddas.GetCFDTime());
          hit.SetTimestamp(ddas.GetTimestamp());
          hit.SetExternalTimestamp(ddas.GetExternalTimestamp());
          //InsertHit(hit);
          fSssd.push_back(hit);
        }

       
    }
  }
  fSize = uml_hits.size();
  return fSize;
}

double TUML::GetSssdEnergy() const {
  double sum = 0.00;
  for(size_t i=0;i<fSssd.size();i++) {
    sum += fSssd.at(i).GetEnergy();
  }
  //std::cout << "sssd energy:   "  << sum << std::endl;

  return sum * GValue::Value("Strip_Slope");
}

double TUML::GetXPosition() const { 
  //return 0;
  if(!fSssd.size()) return -1;

  return (fSssd.at(0).GetChannel()-16 -7.5) *3.15;
  
  /*
  std::vector<double> chan;
  std::vector<double> data;
  for(size_t i=0;i<fSssd.size();i++) {
    chan.push_back(fSssd.at(i).GetChannel()-16);
    data.push_back(fSssd.at(i).Charge());
  }
  return TMath::Mean(chan.begin(),chan.end(),data.begin());
  */
}

double TUML::CalTKE() const {
  return GetPin1().GetEnergy() + 
         GetPin2().GetEnergy() + 
         GetSssdEnergy() +
         GetImplant().GetEnergy() + 
         GValue::Value("TKE_Offset");
}  

double TUML::Beta_to_Gamma(double beta) const {
  if(beta<=0)return 1;
  double beta2 = beta*beta;
  double k = 1.- beta2;
  #define minv 1e-40
  if(fabs(k)< minv)  k=minv;
  return sqrt(1./k);
}



double TUML::SetAoQ() {
   double AoQ_local = -1; 
 if(GetTof()  > 10.) {
//   double dPoPx = GetXPosition()/GValue::Value("Dispersion");
//   double beta = GValue::Value("Length") * (1 + dPoPx * GValue::Value("Disp_Length") /100.)  / GetTof() / GValue::Value("VC");
//   double gamma = Beta_to_Gamma(beta);
   if (beta > 0 && beta < 1 )   {
//       brho  = GValue::Value("Brho0") * ( 1 + dPoPx / 100.);
//       if(Z>0) {
//         double dZ = GValue::Value("Z_disp") - GetZ();
//
//         double dPoPz = sqrt(fabs(dZ))* GValue::Value("Disp_Z") / 100. ;
//         if(dZ<0) dPoPz = -dPoPz;
//    
//         double dPoP = dPoPz + dPoPx;
//
//        brho = GValue::Value("Brho0") * ( 1 + dPoP/100.);
//
//
//       }
     AoQ_local = brho / 3.1071 / beta / gamma;
   } else  {
     gamma=1;
     beta=0;
    }
 }
 return AoQ_local;
}

double TUML::SetZ() const {
  double Z = 0;
  double dPoPx = GetXPosition()/GValue::Value("Dispersion");
  if (GetTof() > 0) {
    double beta = GValue::Value("Length") * (1 + dPoPx * GValue::Value("Disp_Length") /100.)  / GetTof() / GValue::Value("VC");
    double beta2 = beta*beta;
      if (beta2 > 0)
          {
  
          double dE_v = -1.+log(5930./(1./beta2-1.))/beta2;
             if (dE_v > 0) {
	  	//double dE = GetPin1().GetEnergy()*GValue::Value("Tke_Slope0") + GetPin2().GetEnergy()*GValue::Value("Tke_Slope1") + GetSssdEnergy()*GValue::Value("Tke_Slope2") + GValue::Value("TKE_Offset");
                double dE = GetPin1().GetEnergy()+GetPin2().GetEnergy()+GetSssdEnergy();
                double v = sqrt(dE / dE_v);
                Z =  v*v*GValue::Value("Z2_slope") + v* GValue::Value("Z_slope")+GValue::Value("Z_offset"); 
                //int Zi = (double)Z + 0.5;
                //dZ = (double)Z  - Zi;
          }
        }
  }
  //set Z
  return Z;
}



void TUML::CalParameters() {
  TKE = CalTKE();
  //double beta; //!
  double dPoPx = GetXPosition()/GValue::Value("Dispersion");
  brho  = GValue::Value("Brho0") * ( 1 + dPoPx / 100.);
  if(GetTof()>0)
  beta = GValue::Value("Length") * (1 + dPoPx * GValue::Value("Disp_Length") /100.)  / GetTof() / GValue::Value("VC");
  else beta = 0;
  //double gamma; //!
  gamma = Beta_to_Gamma(beta);
  //double brho; //!
  //double Z; //!
  Z = SetZ();
  
  if(Z>0) {
    double dZ = GValue::Value("Z_disp") - GetZ();

    double dPoPz = sqrt(fabs(dZ))* GValue::Value("Disp_Z") / 100. ;
    if(dZ<0) dPoPz = -dPoPz;
    
    double dPoP = dPoPz + dPoPx;

    brho = GValue::Value("Brho0") * ( 1 + dPoP/100.);


  }
  
  //double AoQ; //!
  AoQ   = SetAoQ();
  //double Q; //!
  if(AoQ > 0 && TKE > 0 && gamma>0) {
    Q = TKE / (gamma- 1.) / GValue::Value("AEM") / AoQ;
  }
}


void TUML::ReCalBrho(){
}






