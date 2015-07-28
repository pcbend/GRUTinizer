#include "TGRUTTypes.h"

std::map<std::string, kDetectorSystems> detector_system_map{
  {"Unknown",         kDetectorSystems::UNKNOWN_SYSTEM},
  {"Sega",            kDetectorSystems::SEGA},
  {"Gretina",         kDetectorSystems::GRETINA},
  {"Janus",           kDetectorSystems::JANUS},
  {"Caesar",          kDetectorSystems::CAESAR},
  {"Phoswall",        kDetectorSystems::PHOSWALL},
  {"S800",            kDetectorSystems::S800}
};
