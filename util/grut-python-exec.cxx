#include <iostream>

#include "TPython.h"

#include "LoadGRUTEnv.h"
#include "TGRUTOptions.h"

int main(int argc, char** argv){
  LoadGRUTEnv();
  TGRUTOptions::Get(argc-1, argv+1);

  TPython::ExecScript(argv[1], argc-1, const_cast<const char**>(argv+1));
}
