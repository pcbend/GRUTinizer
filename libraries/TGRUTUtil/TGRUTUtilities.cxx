#include "TGRUTUtilities.h"

#include <fstream>

bool file_exists(const std::string& filename){
  return std::ifstream(filename);
}
