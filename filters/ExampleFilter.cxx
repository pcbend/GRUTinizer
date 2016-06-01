// To make a new filter, copy this file under a new name in the "filter" directory.
// The "FilterCondition" function should return a boolean value.
// The boolean indicates whether the event should be kept or not.

#include "TRuntimeObjects.h"

extern "C"
bool FilterCondition(TRuntimeObjects& obj) {
  return true;
}
