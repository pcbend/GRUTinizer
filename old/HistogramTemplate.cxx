#include "TRuntimeObjects.h"

#include <iostream>

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
//
// Note: from TRuntimeObjects:
//    TList& GetDetectors();
//    TList& GetObjects();
//


extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
}
