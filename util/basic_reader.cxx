

#include <iostream>
#include <cstdio>
#include <fstream>

//#include "GEBStructs.h"
#include "TRawEvent.h"
#include "TRawSource.h"
#include "TRawBanks.h"
#include "Globals.h"

#include "TGretina.h"
#include "TGretinaHit.h"

#include "TSmartBuffer.h"

using namespace std;

int main(int argc, char **argv) {

  if(argc !=2) {
     //printf("try ./.aout Merge###.dat instead\n");
     return 1;
  }

  TRawFileIn infile(argv[1]);
  TRawEvent  event;

  bool foundone =false;

  while(infile.Read(&event)>0) {
   printf("I AM HERE\n");
    switch(event.GetEventType()) {
      case 1:
        event.Print("all"); 
        foundone = true;

        TSmartBuffer buf = event.GetPayloadBuffer();
        TRawEvent::GEBBankType1 raw = *(const TRawEvent::GEBBankType1*)buf.GetData();
        std::cout << raw << std::endl;

        TGretinaHit hit;
        hit.BuildFrom(buf);
        hit.Print("all");

        //gretina data.
        break;
    };
    if(foundone) 
      break;

  }



   printf("\n done.\n\n");
   return 0;
}













