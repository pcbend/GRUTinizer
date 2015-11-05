#include "TRawEventRingSource.h"

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

    return (daqhome + "/bin/ringselector --source=tcp://" + ringname
            + " --sample=PHYSICS_EVENT --non-blocking");
  }
}

TRawEventRingSource::TRawEventRingSource(const std::string& ringname, kFileType file_type)
  : TRawEventPipeSource(generate_ring_command(ringname), file_type),
    fRingName(ringname) { }

std::string TRawEventRingSource::SourceDescription() const {
  return "Ring: " + fRingName;
}
