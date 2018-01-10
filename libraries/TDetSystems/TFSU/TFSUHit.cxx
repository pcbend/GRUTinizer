
#include <TFSUHit.h>

TFSUHit::TFSUHit() { }

TFSUHit::~TFSUHit() { }


void TFSUHit::Clear(Option_t *opt) { TDetectorHit::Clear(); }

void TFSUHit::Print(Option_t *opt) const {

  printf("---------------------------\n");
  printf("TFSUHit @ %lu\n",Timestamp());
  printf("\tAddress:  0x%08x\n",Address());
  printf("\tCharge:   %i\n",Charge());
  printf("\tTime:     %i\n",Time());
  printf("---------------------------\n");
 }


