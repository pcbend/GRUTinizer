#include "TGRUTUtilities.h"

#include <sys/stat.h>
#include <fstream>

bool file_exists(const std::string& filename){
  //std::ifstream(filename);
  struct stat buffer;  
  return (stat(filename.c_str(),&buffer)==0);
}
