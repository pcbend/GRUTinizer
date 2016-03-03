#include "TDetectorEnv.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "TGEBEvent.h"
#include "TGRUTOptions.h"
#include "TNSCLEvent.h"
#include "TRawEvent.h"


TDetectorEnv* TDetectorEnv::env = NULL;

TDetectorEnv& TDetectorEnv::Get(const char* name){

  if(!env){
    env = new TDetectorEnv();
    env->ReadFile(name);
  }

  return *env;
}


void trim(std::string * line, const std::string & trimChars) {
//Removes the the string "trimCars" from  the string 'line'
  if (line->length() == 0)
    return;
  std::size_t found = line->find_first_not_of(trimChars);
  if (found != std::string::npos)
    *line = line->substr(found, line->length());
  found = line->find_last_not_of(trimChars);
  if (found != std::string::npos)
    *line = line->substr(0, found + 1);
  return;
}

Int_t TDetectorEnv::ReadFile(const std::string& filename) {
  this->filename = filename;

  std::ifstream infile;
  infile.open(filename);
  if (!infile) {
    std::cerr << "TDetectorEnv: Could not open file: " << filename << std::endl;
    return -2;
  }

  infile.seekg(0,std::ios::end);
  int length = infile.tellg();
  if(length<1) {
    return -2;
  }

  std::vector<char> buffer(length+1);
  infile.seekg(0,std::ios::beg);
  infile.read(buffer.data(),length);
  buffer[length] = '\0';

  ParseInputData(buffer.data());
  return 1;
}


Int_t TDetectorEnv::ParseInputData(const char *inputdata) {

  std::istringstream infile(inputdata);

  std::string line;
  std::string name;

  //Parse the cal file. This is useful because if the cal file contains something that
  //the parser does not recognize, it just skips it!
  while (std::getline(infile, line)) {
    size_t comment = line.find("//");
    if (comment != std::string::npos) {
      line = line.substr(0, comment);
    }

    comment = line.find("#");
    if (comment != std::string::npos) {
      line = line.substr(0, comment);
    }


    trim(&line);

    if (!line.length())
      continue;

    size_t colon = line.find(":");

    if(colon  == std::string::npos)
      continue;

    std::string type = line.substr(0, colon);
    line = line.substr(colon + 1, line.length());
    trim(&line);
    std::istringstream ss(line);

    kDetectorSystems system = detector_system_map[type];

    int value;
    while (ss >> value) {
      std::vector<int>& id_list = source_ids[system];
      if(value < 0){
        id_list.clear();
        break;
      }

      id_list.push_back(value);
    }
  }

  return 0;
}

// TODO: Implement this.
void TDetectorEnv::Print(Option_t* opt) const { }

void TDetectorEnv::Clear(Option_t* opt){
  source_ids.clear();
}

kDetectorSystems TDetectorEnv::DetermineSystem(int source_id) const {
  kDetectorSystems output = kDetectorSystems::UNKNOWN_SYSTEM;

  for(auto& system : Get().source_ids){
    if(output != kDetectorSystems::UNKNOWN_SYSTEM){
      break;
    }

    for(auto id : system.second){
      if(id == source_id){
        output = system.first;
        break;
      }
    }
  }

  if(output == kDetectorSystems::UNKNOWN_SYSTEM){
    std::cerr << RED << "Unknown sourceid found: " << source_id << RESET_COLOR << std::endl;
  }

  return output;
}

kDetectorSystems TDetectorEnv::DetermineSystem(TRawEvent& event) const {
  int source_id = -1;

  switch(event.GetFileType()){
  case NSCL_EVT:
  {
    TNSCLEvent& nscl_event = (TNSCLEvent&)event;
    source_id = nscl_event.GetSourceID();
  }
    break;

  case ANL_RAW:
  case GRETINA_MODE2:
  case GRETINA_MODE3:
  {
    TGEBEvent& geb_event = (TGEBEvent&)event;
    source_id = geb_event.GetEventType();
  }
  break;

  default:
    break;
  }

  return DetermineSystem(source_id);
}
