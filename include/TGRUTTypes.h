#ifndef _TGRUTTYPES_H_
#define _TGRUTTYPES_H_

#include <map>
#include <string>

#include <Rtypes.h>

enum kDetectorSystems {
  UNKNOWN_SYSTEM = -1,

  GRETINA    = 1,
  MODE3      = 2,
  S800       = 5,
  S800SCALER = 10,
  BANK29     = 8,
  GRETINA_SIM= 11,
  S800_SIM   = 9,

  SEGA = 64,
  JANUS = 65,
  FASTSCINT =4,

  CAESAR = 80,

  PHOSWALL = 17,
  ANL = 14,
  GRAND_RAIDEN = 50,

  NSCLSCALERS = 100
};

extern std::map<std::string, kDetectorSystems> detector_system_map;

enum kFileType {
  UNKNOWN_FILETYPE = -1,
  NSCL_EVT = 1,
  GRETINA_MODE2 = 2,
  GRETINA_MODE3 = 3,
  ANL_RAW = 4,
  RCNP_BLD = 50,
  ROOT_DATA = 256,
  ROOT_MACRO = 257,
  CALIBRATED = 512,
  GVALUE     = 513,
  PRESETWINDOW = 514,
  DETECTOR_ENVIRONMENT = 1024,
  GUI_HIST_FILE = 2048,
  COMPILED_HISTOGRAMS = 2049,
  COMPILED_FILTER = 2053,
  CONFIG_FILE = 2050,
  S800_INVMAP = 2051,
  CUTS_FILE = 2052
};

extern std::map<std::string, EColor> color_system_map;

#endif /* _TGRUTTYPES_H_ */
