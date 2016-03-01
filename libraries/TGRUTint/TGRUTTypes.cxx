#include "TGRUTTypes.h"

#include "TBank29.h"
#include "TCaesar.h"
#include "TFastScint.h"
#include "TJanus.h"
#include "TGretina.h"
#include "TGretSim.h"
#include "TMode3.h"
#include "TPhosWall.h"
#include "TSega.h"
#include "TS800Scaler.h"
#include "TS800Sim.h"

// Map from string to detector enum.
// This is used to parse the DetectorEnvironment.env file.
// This is ONLY used when parsing NSCL data files, not
std::map<std::string, kDetectorSystems> detector_system_map{
  {"Unknown",     kDetectorSystems::UNKNOWN_SYSTEM},
  {"Sega",        kDetectorSystems::SEGA},
  {"Gretina",     kDetectorSystems::GRETINA},
  {"Gretina_Sim", kDetectorSystems::GRETINA_SIM},
  {"Bank29",      kDetectorSystems::BANK29},
  {"Mode3",       kDetectorSystems::MODE3},
  {"Janus",       kDetectorSystems::JANUS},
  {"Caesar",      kDetectorSystems::CAESAR},
  {"Phoswall",    kDetectorSystems::PHOSWALL},
  {"S800",        kDetectorSystems::S800},
  {"S800_SIM",    kDetectorSystems::S800_SIM},
  {"FastScint",   kDetectorSystems::FASTSCINT}
};

// Map from detector enum to detector factory.
// This is used to build each detector type as it is needed.
// Adding a detector to this list allows grutinizer to unpack that detector type.
std::map<kDetectorSystems, TDetectorFactoryBase*> detector_factory_map {
  {kDetectorSystems::GRETINA,     new TDetectorFactory<TGretina>() },
  {kDetectorSystems::GRETINA_SIM, new TDetectorFactory<TGretSim>() },
  {kDetectorSystems::MODE3,       new TDetectorFactory<TMode3>() },
  {kDetectorSystems::S800,        new TDetectorFactory<TS800>() },
  {kDetectorSystems::S800_SIM,    new TDetectorFactory<TS800Sim>() },
  {kDetectorSystems::S800SCALER,  new TDetectorFactory<TS800Scaler>() },
  {kDetectorSystems::BANK29,      new TDetectorFactory<TBank29>() },
  {kDetectorSystems::SEGA,        new TDetectorFactory<TSega>() },
  {kDetectorSystems::JANUS,       new TDetectorFactory<TJanus>() },
  {kDetectorSystems::FASTSCINT,   new TDetectorFactory<TFastScint>() },
  {kDetectorSystems::CAESAR,      new TDetectorFactory<TCaesar>() },
  {kDetectorSystems::PHOSWALL,    new TDetectorFactory<TPhosWall>() }
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
