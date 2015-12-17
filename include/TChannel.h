#ifndef _TCHANNEL__H_
#define _TCHANNEL__H_


#include<string>
#include<map>

#include<TNamed.h>
#include<TList.h>

class TChannel : public TNamed {
public:
  TChannel();
  TChannel(const char*);
  TChannel(const TChannel&);

  virtual ~TChannel();

  void Print(Option_t *opt="") const;
  std::string PrintToString(Option_t *opt="") const;
  void Copy(TObject&)       const;
  void Clear(Option_t *opt ="");
  //int  Compare(const TObject*) const;
  static bool AlphaSort(const TChannel&,const TChannel&);

private:
  unsigned int address;
  int          number;
  std::string  info;
  std::vector<double> energy_coeff;
  std::vector<double> efficiency_coeff;
  //name and title held by TNamed.

  //static std::vector<std::string> fFileNames;
  static std::string fChannelData;

  void SetEnergyCoeff(std::vector<double> tmp)    { energy_coeff    = tmp; }
  void SetEfficiencyCoeff(std::vector<double> tmp) { efficiency_coeff = tmp; }


private:
  static std::map<unsigned int,TChannel*> fChannelMap;
  struct MNEMONIC {
    std::string system;
    int array_position;
    std::string arraysubposition;
    std::string collectedcharge;
    int segment;

    void Unpack(std::string name);
    void Clear(Option_t* opt = "");
  };
  MNEMONIC fMnemonic;

public:
  static TChannel* GetChannel(unsigned int);
  static TChannel* FindChannel(std::string);
  static bool      AddChannel(TChannel*,Option_t *opt="");
  static bool      RemoveChannel(TChannel&);
  bool      AppendChannel(TChannel*);
  bool      ReplaceChannel(TChannel*);

  static int       DeleteAllChannels();
  static int       Size() { return fChannelMap.size(); }


public:
  void SetAddress(unsigned int temp) { address = temp;           }
  void SetName(const char *temp)     {
    TNamed::SetNameTitle(temp,temp);
    fMnemonic.Unpack(temp);
  }
  void SetInfo(const char *temp)     { info.assign(temp); }
  void SetNumber(int temp)           { number = temp;           }

  unsigned int GetAddress() const { return address;           }
  const char*  GetName()    const { return TNamed::GetName(); }
  const char*  GetInfo()    const { return info.c_str();      }
  int          GetNumber()  const { return number;            }
  int GetArrayPosition() const { return fMnemonic.array_position; }
  int GetSegment() const { return fMnemonic.segment; }
  const char* GetSystem() const { return fMnemonic.system.c_str(); }
  const char* GetArraySubposition() const { return fMnemonic.arraysubposition.c_str(); }
  const char* GetCollectedCharge() const { return fMnemonic.collectedcharge.c_str(); }

  std::vector<double> GetEnergyCoeff()    const { return energy_coeff; }
  void                AddEnergyCoeff(double  tmp)    { energy_coeff.push_back(tmp); }
  std::vector<double> GetEfficiencyCoeff() const { return efficiency_coeff; }
  void                AddEfficiencyCoeff(double  tmp) { efficiency_coeff.push_back(tmp); }

  void DestroyEnergyCoeff()    { energy_coeff.clear(); }
  void DestroyEfficiencyCoeff() { efficiency_coeff.clear(); }

  void DestroyCalibrations();

  double CalEnergy(int);
  double CalEnergy(double);

  //double CalEfficiency(int);
  //double CalEfficiency(double) const;

public:
  static int ReadCalFile(const char* filename="",Option_t *opt="replace");
  static int WriteCalFile(std::string filename="",Option_t *opt="");

private:
  static int  ParseInputData(std::string &input,Option_t *opt);
  static int  WriteToBuffer(Option_t *opt);
  static void trim(std::string *, const std::string &trimChars=" \f\n\r\t\v");

  ClassDef(TChannel,1);
};

#endif
