
#include <cstdio>

#include "TChannel.h"

int main(int argc,char **argv) {
  TChannel *channel;
  for(int i=1;i<37;i++) {  
    char buffer[10];
    char* crystals = "ABCD";
    char* gainranges = "ABCD";

    for(int cryspos=0; cryspos<4; cryspos++){
      for(int gainrange=0; gainrange<4; gainrange++){
        char crysletter = crystals[cryspos];
        char gainrangeletter = gainranges[gainrange];
        sprintf(buffer,"GRG%02i%c%c00",i,crysletter,gainrangeletter);
        channel = new TChannel(buffer);
        int address = (1<<24) + ((i*4+cryspos)<<16) + gainrange;
        channel->SetAddress(address);
        channel->SetNumber((i*4)+0);
        channel->AddEnergyCoeff(0.0);
        channel->AddEnergyCoeff(1.0);
        channel->AddChannel(channel);
        printf("%si created\n",buffer);
      }
    }



  }
  printf("I AM HERE\n");
  TChannel::WriteCalFile("polariztion_cal.cal");

  return 0; 
}































