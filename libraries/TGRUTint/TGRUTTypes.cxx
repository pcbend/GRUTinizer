#include "TGRUTTypes.h"

std::map<std::string, kDetectorSystems> detector_system_map{
  {"Unknown",         kDetectorSystems::UNKNOWN_SYSTEM},
  {"Sega",            kDetectorSystems::SEGA},
  {"Gretina",         kDetectorSystems::GRETINA},
  {"Gretina_Sim",     kDetectorSystems::GRETINA_SIM},
  {"Bank88",          kDetectorSystems::BANK88},
  {"Mode3",           kDetectorSystems::MODE3},
  {"Janus",           kDetectorSystems::JANUS},
  {"Caesar",          kDetectorSystems::CAESAR},
  {"Phoswall",        kDetectorSystems::PHOSWALL},
  {"S800",            kDetectorSystems::S800},
  {"S800_SIM",        kDetectorSystems::S800_SIM},
  {"FastScint",       kDetectorSystems::FASTSCINT},
  {"FSU",             kDetectorSystems::FSU},
  {"UML",             kDetectorSystems::UML},
  {"OBJ",             kDetectorSystems::OBJ}
};



std::map<std::string, EColor> color_system_map {
  {"kWhite",   EColor::kWhite},   
  {"kBlack",   EColor::kBlack},   
  {"kGray",    EColor::kGray},    
  {"kRed",     EColor::kRed},     
  {"kGreen",   EColor::kGreen},   
  {"kBlue",    EColor::kBlue},    
  {"kYellow",  EColor::kYellow},  
  {"kMagenta", EColor::kMagenta}, 
  {"kCyan",    EColor::kCyan},    
  {"kOrange",  EColor::kOrange},  
  {"kSpring",  EColor::kSpring},  
  {"kTeal",    EColor::kTeal},    
  {"kAzure",   EColor::kAzure},   
  {"kViolet",  EColor::kViolet},  
  {"kPink",    EColor::kPink} 
};



