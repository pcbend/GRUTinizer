#include "TGRUTUtilities.h"

#include <sys/stat.h>
#include <fstream>

bool file_exists(const char *filename){
  //std::ifstream(filename);
  struct stat buffer;  
  return (stat(filename,&buffer)==0);
}
