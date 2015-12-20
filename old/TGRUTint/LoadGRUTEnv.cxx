#include "LoadGRUTEnv.h"

#include <cstdlib>

#include "TEnv.h"

#include "ProgramPath.h"

void LoadGRUTEnv() {
  // Set the GRUTSYS variable based on the executable path.
  // If GRUTSYS has already been defined, don't overwrite.
  setenv("GRUTSYS", (program_path()+"/..").c_str(), 0);
  std::string grut_path = program_path() + "/../.grutrc";
  gEnv->ReadFile(grut_path.c_str(),kEnvChange);
}
