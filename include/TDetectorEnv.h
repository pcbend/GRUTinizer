#ifndef _TDETECTORENV_H_
#define _TDETECTORENV_H_

#include "TNamed.h"

#include "TGRUTTypes.h"

class TRawEvent;

class TDetectorEnv : public TNamed {
public:
  static TDetectorEnv& Get(const char* name = "");

  static bool Caesar()     { return Get().source_ids[kDetectorSystems::CAESAR].size(); }
  static bool Gretina()    { return Get().source_ids[kDetectorSystems::GRETINA].size(); }
  static bool Bank29()     { return Get().source_ids[kDetectorSystems::BANK29].size(); }
  static bool Mode3()      { return Get().source_ids[kDetectorSystems::MODE3].size(); }
  static bool Janus()      { return Get().source_ids[kDetectorSystems::JANUS].size(); }
  static bool Phoswall()   { return Get().source_ids[kDetectorSystems::PHOSWALL].size(); }
  static bool S800()       { return Get().source_ids[kDetectorSystems::S800].size(); }
  static bool S800Scaler() { return Get().source_ids[kDetectorSystems::S800SCALER].size(); }
  static bool Sega()       { return Get().source_ids[kDetectorSystems::SEGA].size(); }
  static bool FastScint()  { return Get().source_ids[kDetectorSystems::FASTSCINT].size(); }
  static bool Lenda()      { return Get().source_ids[kDetectorSystems::LENDA].size(); }

  static const std::vector<int>& CaesarIDs()     { return Get().source_ids[kDetectorSystems::CAESAR]; }
  static const std::vector<int>& SegaIDs()       { return Get().source_ids[kDetectorSystems::SEGA]; }
  static const std::vector<int>& S800IDs()       { return Get().source_ids[kDetectorSystems::S800]; }
  static const std::vector<int>& S800ScalerIDs() { return Get().source_ids[kDetectorSystems::S800SCALER]; }
  static const std::vector<int>& PhoswallIDs()   { return Get().source_ids[kDetectorSystems::PHOSWALL]; }
  static const std::vector<int>& JANUSIDs()      { return Get().source_ids[kDetectorSystems::JANUS]; }
  static const std::vector<int>& GretinaIDs()    { return Get().source_ids[kDetectorSystems::GRETINA]; }
  static const std::vector<int>& Mode3IDs()      { return Get().source_ids[kDetectorSystems::MODE3]; }
  static const std::vector<int>& Bank29IDs()     { return Get().source_ids[kDetectorSystems::BANK29]; }
  static const std::vector<int>& FastScintIDs()  { return Get().source_ids[kDetectorSystems::FASTSCINT]; }
  static const std::vector<int>& LendaIDs()      { return Get().source_ids[kDetectorSystems::LENDA]; }

  kDetectorSystems DetermineSystem(int source_id) const;
  kDetectorSystems DetermineSystem(TRawEvent& event) const;

  Int_t ReadFile(const std::string& filename);
  Int_t ParseInputData(const char* inputdata);
  void Print(Option_t* opt = "") const;
  void Clear(Option_t* opt = "");

//private:
  static TDetectorEnv* env;

  TDetectorEnv() { }

  std::map<kDetectorSystems, std::vector<int> > source_ids;
  std::string filename;

  ClassDef(TDetectorEnv, 1);
};

void trim(std::string *, const std::string & trimChars = " \f\n\r\t\v");

#endif /* _TDETECTORENV_H_ */
