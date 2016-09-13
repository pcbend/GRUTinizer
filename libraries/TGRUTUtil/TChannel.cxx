#include "TChannel.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>

#include "TRandom.h"

#include "GRootFunctions.h"

std::map<unsigned int,TChannel*> TChannel::fChannelMap;
TChannel *TChannel::fDefaultChannel = new TChannel("TChannel",0xffffffff);
std::string TChannel::fChannelData;
std::vector<double> TChannel::empty_vec;

ClassImp(TChannel)

TChannel::TChannel() {
   Clear();
   SetName("TChannels");
   TNamed::SetTitle("TChannel Data");
}

TChannel::TChannel(const char *name) {
   Clear();
   SetName(name);
}

TChannel::TChannel(const char *name,unsigned int address) {
   Clear();
   SetName(name);
   SetAddress(address);
}

TChannel::TChannel(const TChannel& rhs)
  : TNamed(rhs) {
  rhs.Copy(*this);
}

TChannel::~TChannel() { }

std::ostream& operator<<(std::ostream& out, const TChannel& chan) {
  // Print out the standard stuff
  out << chan.GetName() << ":  {\n"
      << "   Address:\t0x" << std::hex << std::setfill('0') << std::setw(8) << chan.address << std::dec << "\n"
      << "   Number:\t" << chan.number << "\n"
      << "   Info:\t" << chan.info << "\n"
      << "   System:\t" << chan.system << "\n"
      << "   Position:\t" << chan.array_position << "\n"
      << "   Subposition:\t" << chan.array_subposition << "\n"
      << "   Segment:\t" << chan.segment << "\n"
      //<< "   Collected_Charge:\t" << chan.collected_charge << "\n"
      << "   Pedestal:\t" << chan.pedestal << "\n";

  // Print out each energy coefficient
  for(auto& start_coeff : chan.energy_coeff) {
    out << "   EnergyCoeff";
    if(start_coeff.start_time != -DBL_MAX) {
      out << "[" << start_coeff.start_time << "]";
    }
    out << ":";
    for(double coef : start_coeff.coefficients) {
      out << "\t" << coef;
    }
    out << "\n";
  }

  // Print out each time coefficient
  for(auto& start_coeff : chan.time_coeff) {
    out << "   TimeCoeff";
    if(start_coeff.start_time != -DBL_MAX) {
      out << "[" << start_coeff.start_time << "]";
    }
    out << ":";
    for(double coef : start_coeff.coefficients) {
      out << "\t" << coef;
    }
    out << "\n";
  }

  // Print out each efficiency coefficient
  out << "   EfficiencyCoeff:";
  for(double coef : chan.efficiency_coeff) {
    out << "\t" << coef;
  }
  out << "\n";

  // Close out the rest
  out << "}\n-----------------------------------\n";

  return out;
}

void TChannel::Print(Option_t *opt) const {
  std::cout << *this;
}

std::string TChannel::PrintToString(Option_t *opt) const {
  std::stringstream output;
  output << *this;
  return output.str();
}

void TChannel::Copy(TObject &rhs) const {
  TNamed::Copy(rhs);
  ((TChannel&)rhs).address = address;
  ((TChannel&)rhs).number  = number;
  ((TChannel&)rhs).info    = info;
  ((TChannel&)rhs).system    = system;
  ((TChannel&)rhs).array_position    = array_position;
  ((TChannel&)rhs).array_subposition    = array_subposition;
  ((TChannel&)rhs).collected_charge    = collected_charge;
  ((TChannel&)rhs).segment    = segment;
  ((TChannel&)rhs).energy_coeff = energy_coeff;
  ((TChannel&)rhs).time_coeff = time_coeff;
  ((TChannel&)rhs).efficiency_coeff = efficiency_coeff;
  ((TChannel&)rhs).pedestal = pedestal;
}

void TChannel::Clear(Option_t *opt) {
  TNamed::Clear(opt);
  address         = 0xffffffff;
  number          = -1;
  info.clear();
  ClearCalibrations();
  array_position = -1;
  segment = -1;
  system = "";
  array_subposition = "";
  collected_charge = "";
  pedestal = 0;
}

//void TChannel::Compare(const TObject &rhs) const { }


bool TChannel::AlphaSort(const TChannel &chana,const TChannel &chanb) {
   //Compares the names of the two TChannels. Returns true if the names are the
   //same, false if different.
   std::string namea;
   namea.assign(((TChannel&)chana).GetName());
   if(namea.compare(((TChannel&)chanb).GetName()) <= 0) {
     return true;
   } else {
     return false;
   }
}


TChannel* TChannel::GetChannel(unsigned int add)   {
  TChannel *chan = 0;
  if(add==0xffffffff)
    return fDefaultChannel;
  if(fChannelMap.count(add)==1) {
    chan = fChannelMap.at(add);
  }
  return chan;
}

TChannel* TChannel::Get(unsigned int add)   {
  return GetChannel(add);
}



TChannel* TChannel::FindChannel(std::string name)   {
  TChannel *chan = 0;
  if(name.length()==0)
     return chan;
  for(auto &iter : fChannelMap) {
     chan = iter.second;
     if(!name.compare(chan->GetName()))
        break;
     chan = 0;
  }
  return chan;
}

bool TChannel::AddChannel(TChannel* chan,Option_t *opt) {
  if(!chan)
     return false;
  TString option(opt);
  if(fChannelMap.count(chan->GetAddress())==1) {
     if(option.Contains("overwrite",TString::kIgnoreCase)) {
       TChannel *oldchan = GetChannel(chan->GetAddress());
       chan->ReplaceChannel(oldchan);
       return true;
     } else {
       fprintf(stderr,"%s: Trying to add a channel that already exists!\n",__PRETTY_FUNCTION__);
       return false;
     }
  } else if(chan->GetAddress()==0xffffffff) { //default chan, get rid of it and ignore;
     delete chan;
     chan = 0;
  } else {
    fChannelMap.insert(std::make_pair(chan->GetAddress(),chan));
  }
  return true;
}

bool TChannel::AppendChannel(TChannel *oldchan) {
  //address         = 0xffffffff;
  if(strlen(this->GetName()))
     oldchan->SetName(this->GetName());
  if(this->GetNumber() != -1)
     oldchan->SetNumber(this->GetNumber());
  if(strlen(this->GetInfo()))
     oldchan->SetInfo(this->GetInfo());
  if(this->GetTimeCoeff().size()>0)
     oldchan->SetTimeCoeff(this->GetTimeCoeff());
  if(this->GetEnergyCoeff().size()>0)
     oldchan->SetEnergyCoeff(this->GetEnergyCoeff());
  if(this->GetEfficiencyCoeff().size()>0)
     oldchan->SetEfficiencyCoeff(this->GetEfficiencyCoeff());
  return true;
}

bool TChannel::ReplaceChannel(TChannel *oldchan)  {
  this->Copy(*oldchan);
  return true;
}

bool TChannel::RemoveChannel(TChannel &chan) {
  if(fChannelMap.count(chan.GetAddress()==1)) {
    fChannelMap.erase(chan.GetAddress());
    return true;
  }
  return false;
}

int TChannel::DeleteAllChannels()  {
  int count = 0;
  for(auto &iter : fChannelMap) {
    if(iter.second)
      delete iter.second;
    iter.second = 0;
    count++;
  }
  fChannelMap.clear();
  return count;
}



void TChannel::trim(std::string * line, const std::string & trimChars) {
   //Removes the the string "trimCars" from the start or end of 'line'
  if(line->length() == 0)
    return;

  std::size_t found = line->find_first_not_of(trimChars);
  if(found != std::string::npos)
    *line = line->substr(found, line->length());

  found = line->find_last_not_of(trimChars);
  if(found != std::string::npos)
    *line = line->substr(0, found + 1);
}

void TChannel::ClearCalibrations() {
  ClearEnergyCoeff();
  ClearEfficiencyCoeff();
  ClearTimeCoeff();
}

const std::vector<double>& TChannel::GetEnergyCoeff(double timestamp) const {
  for(auto& coeff_time : energy_coeff){
    if(timestamp >= coeff_time.start_time) {
      return coeff_time.coefficients;
    }
  }
  // Should never reach here, but just in case.
  return empty_vec;
}

void TChannel::ClearEnergyCoeff() {
  energy_coeff.clear();
  energy_coeff.push_back({std::vector<double>(), -DBL_MAX});
}

void TChannel::SetEnergyCoeff(std::vector<double> coeff, double timestamp) {
  std::vector<double>* found = NULL;
  for(auto& ec : energy_coeff) {
    if(ec.start_time == timestamp){
      found = &ec.coefficients;
    }
  }

  if(found){
    *found = std::move(coeff);
  } else {
    energy_coeff.push_back({std::move(coeff), timestamp});
    std::sort(energy_coeff.begin(), energy_coeff.end());
  }
}

double TChannel::CalEnergy(int charge, double timestamp) const {
  return Calibrate(charge-pedestal, GetEnergyCoeff(timestamp));
}

double TChannel::CalEnergy(double charge, double timestamp) const {
  return Calibrate(charge-pedestal, GetEnergyCoeff(timestamp));
}

double TChannel::CalEfficiency(double energy) const {
  return Efficiency(energy,GetEfficiencyCoeff());
}


const std::vector<double>& TChannel::GetTimeCoeff(double timestamp) const {
  for(auto& tc : time_coeff) {
    if(timestamp >= tc.start_time) {
      return tc.coefficients;
    }
  }
  // Should never reach here, but just in case.
  return empty_vec;
}

void TChannel::ClearTimeCoeff() {
  time_coeff.clear();
  time_coeff.push_back({std::vector<double>(), -DBL_MAX});
}

void TChannel::SetTimeCoeff(std::vector<double> coeff, double timestamp) {
  std::vector<double>* found = NULL;
  for(auto& tc : time_coeff) {
    if(tc.start_time == timestamp){
      found = &tc.coefficients;
    }
  }

  if(found){
    *found = std::move(coeff);
  } else {
    time_coeff.push_back({std::move(coeff), timestamp});
    std::sort(time_coeff.begin(), time_coeff.end());
  }
}


double TChannel::CalTime(int time, double timestamp) const {
  return Calibrate(time, GetTimeCoeff(timestamp));
}

double TChannel::CalTime(double time, double timestamp) const {
  return Calibrate(time, GetTimeCoeff(timestamp));
}

void TChannel::ClearEfficiencyCoeff() {
  efficiency_coeff.clear();
}

double TChannel::Calibrate(int value, const std::vector<double>& coeff) {
  if(value==0){
    return 0;
  }

  double dvalue = value + gRandom->Uniform();
  return Calibrate(dvalue, coeff);
}

double TChannel::Calibrate(double value, const std::vector<double>& coeff) {
  if(coeff.size() == 0){
    return value;
  }

  // Evaluate the polynomial using Horner's Method
  double cal_value = 0;
  for(int i=coeff.size()-1; i>=0; i--){
    cal_value *= value;
    cal_value += coeff[i];
  }
  return cal_value;
}

double TChannel::Efficiency(double energy,const std::vector<double>& coeff) {
  // EFF = 10^(A0 + A1*LOG(E) + A2*LOG(E)^2 + A3/E^2);
  if(coeff.size()<4) {
    return 0.0;
  }
  return GRootFunctions::GammaEff(&energy,const_cast<double*>(&coeff[0]));
}






int TChannel::ReadCalFile(const char* filename,Option_t *opt) {
  std::string infilename = filename;
  if(infilename.length()==0)
     return -1;

  std::ifstream infile;
  infile.open(infilename.c_str());
  if(!infile) {
     fprintf(stderr,"%s:  could not open infile %s.",__PRETTY_FUNCTION__,infilename.c_str());
     return -2;
  }
  infile.seekg(0,std::ios::end);
  size_t length = infile.tellg();
  if(length==0) {
    fprintf(stderr,"%s:  infile %s appears to be empty.",__PRETTY_FUNCTION__,infilename.c_str());
    return -2;
  }

  std::string sbuffer;
  std::vector<char> buffer(length);
  infile.seekg(0,std::ios::beg);
  infile.read(buffer.data(),(int)length);
  sbuffer.assign(buffer.data());

  int channels_found = ParseInputData(sbuffer,opt);
  if(channels_found) {
    //fFileNames.push_back(std::string(filename);;
    fChannelData = sbuffer; //.push_back(std::string((const char*)buffer);
  }
  return channels_found;
}

int TChannel::WriteCalFile(std::string outfilename,Option_t *opt) {
  std::vector<TChannel> chanvec;
  for(auto &iter : fChannelMap) {
    if(iter.second)
       chanvec.push_back(*iter.second);
  }
  std::sort(chanvec.begin(),chanvec.end(),TChannel::AlphaSort);

  //std::string output;
  //for(auto iter : chanvec) {
  //  output.append(iter->PrintToString());
  //  output.append("\n");
  //}
  int count =0;
  if(outfilename.length()>0) {
    std::ofstream calout;
    calout.open(outfilename.c_str());
    for(auto &iter : chanvec) {
      calout << iter.PrintToString(opt);
      calout << std::endl;
    }
    calout << std::endl;
    calout.close();
    count ++;
  } else {
    for(auto &iter : chanvec) {
      iter.Print(opt);
      count++;
    }
  }
  return count;
}

int TChannel::WriteToBuffer(Option_t *opt) {
  std::vector<TChannel> chanvec;
  for(auto &iter : fChannelMap) {
    if(iter.second)
       chanvec.push_back(*iter.second);
  }
  std::sort(chanvec.begin(),chanvec.end(),TChannel::AlphaSort);

  fChannelData.clear();
  for(auto &iter : chanvec) {
    fChannelData.append(iter.PrintToString());
    fChannelData.append("\n");
  }
  return 0;
}


int TChannel::ParseInputData(std::string &input,Option_t *opt) {
  std::istringstream infile(input);
  TChannel *channel = 0;
  std::string line;
  int linenumber = 0;
  int newchannels = 0;

  bool brace_open = false;
  std::string name;

  while(std::getline(infile,line)) {
    linenumber++;
    trim(&line);
    size_t comment = line.find("//");
    if(comment != std::string::npos) {
       line = line.substr(0,comment);
    }
    if(line.length()==0)
      continue;

    size_t openbrace  = line.find("{");
    size_t closebrace = line.find("}");
    size_t colon      = line.find(":");

    //=============================================//
    if(openbrace  == std::string::npos &&
       closebrace == std::string::npos &&
       colon      == std::string::npos) {
      continue;
    }
    //=============================================//
    if(closebrace != std::string::npos) {
      brace_open = false;
      if(channel) {
         TChannel *currentchan = GetChannel(channel->GetAddress());
         if(!currentchan) {
           AddChannel(channel);
           newchannels++;
         } else {
           channel->AppendChannel(currentchan);
           delete channel;
           newchannels++;
         }
      }
      channel =0;
      name.clear();
    }
    //=============================================//
    if(openbrace != std::string::npos) {
       brace_open = true;
       name = line.substr(0,openbrace).c_str();
       trim(&name);
       channel = new TChannel(name.c_str());
    }
    //=============================================//
    if(brace_open) {
      if(colon != std::string::npos) {
        // Type is everything up to the colon
        std::string type = line.substr(0,colon);
        trim(&type);
        for(unsigned int i=0; i<type.length(); i++){
          type[i] = toupper(type[i]);
        }

        // The payload is everything after the colon
        line = line.substr(colon+1,line.length());
        trim(&line);
        std::istringstream ss(line);

        if(type == "NAME") {
          channel->SetName(line.c_str());

        } else if(type == "ADDRESS") {
          unsigned int add = 0; ss >> add;
          if(add==0) {
            std::stringstream newss(line);
            newss << std::hex;
            newss >> add;
          }
          channel->SetAddress(add);

        } else if(type == "NUMBER") {
          int num = 0; ss >> num;
          channel->SetNumber(num);

        } else if(type == "INFO") {
          channel->SetInfo(line.c_str());

        } else if(type.find("ENERGYCOEFF")==0 ||
                  type.find("ENGCOEFF")==0) {
          channel->SetEnergyCoeff(ParseListOfDoubles(ss),
                                  ParseStartTime(type));

        } else if(type == "PEDESTAL") {
          int val = 0; ss >> val;
          channel->SetPedestal(val);

        } else if(type.find("TIMECOEFF")==0) {
          channel->SetTimeCoeff(ParseListOfDoubles(ss),
                                ParseStartTime(type));

        } else if((type == "EFFICIENCEYCOEFF") ||
                  (type == "EFFCOEFF")) {
          channel->efficiency_coeff = ParseListOfDoubles(ss);

        } else if(type == "SYSTEM") {
          channel->system = line;

        } else if(type == "POSITION") {
          ss >> channel->array_position;

        } else if(type == "SUBPOSITION") {
          channel->array_subposition = line;

        } else if(type == "COLLECTED_CHARGE") {
          channel->collected_charge = line;

        } else if(type == "SEGMENT") {
          ss >> channel->segment;
        }
      }
    }
  }

  if(!strcmp(opt,"debug"))
     printf("parsed %i lines,\n",linenumber);

  return newchannels;
}

std::vector<double> TChannel::ParseListOfDoubles(std::istream& ss) {
  std::vector<double> output;
  std::string text;
  while(ss >> text) {
    output.push_back(std::atof(text.c_str()));
  }
  return output;
}

double TChannel::ParseStartTime(const std::string& type) {
  size_t open_bracket = type.find("[");
  size_t close_bracket = type.find("]");
  if(open_bracket == std::string::npos ||
     close_bracket == std::string::npos) {
    return -DBL_MAX;
  }

  return std::atof(type.c_str() + open_bracket + 1);
}

void TChannel::Streamer(TBuffer &R__b) {
  this->SetBit(kCanDelete);
  //printf("TChannel Streamer.\n");
  UInt_t R__s, R__c;
  if(R__b.IsReading()) {
     Version_t R__v = R__b.ReadVersion(&R__s,&R__c);
     TNamed::Streamer(R__b);
     if(R__v>1) { }
     { TString R__str; R__str.Streamer(R__b); fChannelData.assign(R__str.Data()); }
     ParseInputData(fChannelData);
     R__b.CheckByteCount(R__s,R__c,TChannel::IsA());
  } else {
     R__c = R__b.WriteVersion(TChannel::IsA(),true);
     TNamed::Streamer(R__b);
     WriteToBuffer();
     { TString R__str = fChannelData.c_str(); R__str.Streamer(R__b);}
     R__b.SetByteCount(R__c,true);
  }
}

void TChannel::UnpackMnemonic(std::string name) {
  if(name.length()<9) {
    return;
  }
  std::string buf;

  system.assign(name,0,3);

  buf.clear(); buf.assign(name,3,2);
  array_position = (uint16_t)atoi(buf.c_str());
  array_subposition.assign(name,5,1);
  collected_charge.assign(name,6,1);
  buf.clear(); buf.assign(name,7,2);
  segment = (uint16_t)atoi(buf.c_str());
}
