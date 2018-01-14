#ifndef _TCHANNEL__H_
#define _TCHANNEL__H_

#include <cfloat>
#include <iostream>
#include <string>
#include <map>

#include <TNamed.h>
#include <TList.h>
#include <TVector3.h>

class TChannel : public TNamed {
public:
  static bool AlphaSort(const TChannel&,const TChannel&);
  static TChannel* GetChannel(unsigned int);
  static TChannel* Get(unsigned int=0xffffffff);
  static TChannel* FindChannel(std::string);
  static bool AddChannel(TChannel*,Option_t *opt="");
  static bool RemoveChannel(TChannel&);
  static int DeleteAllChannels();
  static int Size() { return fChannelMap.size(); }
  static int ReadCalFile(const char* filename="",Option_t *opt="replace");
  static int WriteCalFile(std::string filename="",Option_t *opt="");


  TChannel();
  TChannel(const char*);
  TChannel(const char*,unsigned int);
  TChannel(const TChannel&);

  virtual ~TChannel();

  void Print(Option_t *opt="") const;
  std::string PrintToString(Option_t *opt="") const;
  void Copy(TObject&)       const;
  void Clear(Option_t *opt ="");


  unsigned int GetAddress() const          { return address; }
  const char*  GetName() const             { return TNamed::GetName(); }
  const char*  GetInfo() const             { return info.c_str(); }
  int          GetNumber() const           { return number; }
  int          GetArrayPosition() const    { return array_position; }
  int          GetSegment() const          { return segment; }
  const char*  GetSystem() const           { return system.c_str(); }
  const char*  GetArraySubposition() const { return array_subposition.c_str(); }
  const char*  GetCollectedCharge() const  { return collected_charge.c_str(); }
  double       GetPedestal() const         { return pedestal; }

  TVector3     GetChannelPosition() const  { return fPosition; }

  void SetAddress(unsigned int temp) { address = temp; }
  void SetName(const char *temp)     {
    TNamed::SetNameTitle(temp,temp);
    UnpackMnemonic(temp);
  }
  void SetInfo(const char *temp) { info.assign(temp); }
  void SetNumber(int temp) { number = temp; }
  void SetPedestal(int value) { pedestal = value; }

  void ClearCalibrations();

  void SetEnergyCoeff(std::vector<double> coeff, double timestamp=-DBL_MAX);
  const std::vector<double>& GetEnergyCoeff(double timestamp=-DBL_MAX) const;
  void ClearEnergyCoeff();
  double CalEnergy(int charge, double timestamp=-DBL_MAX) const;
  double CalEnergy(double charge, double timestamp=-DBL_MAX) const;

  void SetTimeCoeff(std::vector<double> tmp, double timestamp=-DBL_MAX);
  const std::vector<double>& GetTimeCoeff(double timestamp=-DBL_MAX) const;
  void ClearTimeCoeff();
  double CalTime(int tdc, double timestamp=-DBL_MAX) const;
  double CalTime(double tdc, double timestamp=-DBL_MAX) const;

  void SetEfficiencyCoeff(std::vector<double> tmp) { efficiency_coeff = tmp; }
  std::vector<double> GetEfficiencyCoeff() const { return efficiency_coeff; }
  void AddEfficiencyCoeff(double tmp) { efficiency_coeff.push_back(tmp); }
  void ClearEfficiencyCoeff();
  double CalEfficiency(double energy) const;


  bool AppendChannel(TChannel*);
  bool ReplaceChannel(TChannel*);

  friend std::ostream& operator<<(std::ostream& out, const TChannel& chan);

//private:
public:
  static int  ParseInputData(std::string &input,Option_t *opt="");
  static int  WriteToBuffer(Option_t *opt="");
  static std::vector<double> ParseListOfDoubles(std::istream& ss);

  static double Calibrate(int value, const std::vector<double>& coeff);
  static double Calibrate(double value, const std::vector<double>& coeff);
  static double ParseStartTime(const std::string& type);
  static double Efficiency(double energy, const std::vector<double>& coeff);



  static std::vector<double> empty_vec; //!

  unsigned int address;
  int          number;
  std::string  info;

  std::string system;
  int array_position;
  std::string array_subposition;
  std::string collected_charge;
  int segment;

  void SetChannelX(double x) { fPosition.SetX(x); }
  void SetChannelY(double y) { fPosition.SetY(y); }
  void SetChannelZ(double z) { fPosition.SetZ(z); }


  TVector3 fPosition;

  // For backwards compatibility, unpack the name
  //  into the system/array_position/arraysubposition/collectedcharge/segment.
  void UnpackMnemonic(std::string name);

  struct CoefficientTimes {
    std::vector<double> coefficients;
    double start_time;

    bool operator<(const CoefficientTimes& other) const {
      return start_time > other.start_time;
    }
  };

  std::vector<CoefficientTimes> energy_coeff;
  std::vector<CoefficientTimes> time_coeff;
  std::vector<double> efficiency_coeff;
  int pedestal;
  //name and title held by TNamed.

  //static std::vector<std::string> fFileNames;
  static std::string fChannelData;
  static std::map<unsigned int,TChannel*> fChannelMap;
  static TChannel *fDefaultChannel;

  

  ClassDef(TChannel,3);
};

#endif
