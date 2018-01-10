#include "TRawSource.h"

#include <cstdlib>
#include <iostream>

namespace {
  std::string generate_ring_command(std::string ringname){
    if(ringname.find("/") == std::string::npos){
      ringname = "localhost/" + ringname;
    }

    const char* daqhome_cstr = std::getenv("DAQHOME");
    std::string daqhome;
    if(daqhome_cstr){
      daqhome = daqhome_cstr;
    } else {
      daqhome = "/usr/opt/nscldaq/current";
      std::cerr << "Environment variable DAQHOME not set, assuming DAQHOME=" << daqhome << std::endl;
    }

    std::string command = (daqhome + "/bin/ringselector --source=tcp://" + ringname
                           + " --sample=PHYSICS_EVENT --non-blocking");
    return command;
  }
}

TRingByteSource::TRingByteSource(const std::string& ringname)
  : TPipeByteSource(generate_ring_command(ringname)),
    fRingName(ringname) { }

std::string TRingByteSource::SourceDescription(bool /*long_description*/) const {
  return "Ring: " + fRingName;
}
