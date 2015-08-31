#ifndef _TGRUTTYPES_H_
#define _TGRUTTYPES_H_

#include <map>
#include <string>

enum kDetectorSystems {
  UNKNOWN_SYSTEM = -1,

  GRETINA = 1,
  MODE3   = 2,
  S800 = 5,

  SEGA = 64,
  JANUS = 65,

  CAESAR = 80,

  PHOSWALL = 17
};

extern std::map<std::string, kDetectorSystems> detector_system_map;

enum kFileType {
  UNKNOWN_FILETYPE = -1,
  NSCL_EVT = 1,
  GRETINA_MODE2 = 2,
  GRETINA_MODE3 = 3,
  ROOT_DATA = 256,
  ROOT_MACRO = 257,
  CALIBRATED = 512,
  DETECTOR_ENVIRONMENT = 1024
};

#endif /* _TGRUTTYPES_H_ */
