#include <iostream>

#include "TEnv.h"
#include "TPython.h"
#include "TString.h"

#include "ProgramPath.h"
#include "TGRUTOptions.h"

int main(int argc, char** argv){
  // Set the GRUTSYS variable based on the executable path.
  // If GRUTSYS has already been defined, don't overwrite.
  setenv("GRUTSYS", (program_path()+"/..").c_str(), 0);
  std::string grut_path = Form("%s/.grutrc",getenv("GRUTSYS")); // + "/../.grutrc";
  gEnv->ReadFile(grut_path.c_str(),kEnvChange);


  TGRUTOptions::Get(argc-1, argv+1);

  if(argc > 1){
    TPython::ExecScript(argv[1], argc-1, const_cast<const char**>(argv+1));
  } else {
    TPython::Prompt();
  }
}
