
#include <TChannel.h>
#include <TRandom.h>

#include <utility>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

std::map<unsigned int,TChannel*> TChannel::fChannelMap;
std::string TChannel::fChannelData;

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

TChannel::TChannel(const TChannel& rhs) {
  rhs.Copy(*this);
}

TChannel::~TChannel() { }

void TChannel::Print(Option_t *opt) const {
  printf("%s:  {\n",GetName());
  printf("Address: \t0x%08x\n",address);
  printf("Number:  \t%i\n",number);
  printf("Info:    \t%s\n",info.c_str());
  printf("EnergyCoeff:  ");
  for(auto &i : energy_coeff) { printf("\t%.04f",i); }
  printf("\n");
  printf("EfficienyCoeff:");
  for(auto &i : efficiency_coeff) { printf("\t%.04f",i); }
  printf("\n}\n-----------------------------------\n");
  return;
}

std::string TChannel::PrintToString(Option_t *opt) const {
  std::string output;
  output.append(Form("%s:  {\n",GetName()));
  output.append(Form("Address: \t0x%08x\n",address));
  output.append(Form("Number:  \t%i\n",number));
  output.append(Form("Info:    \t%s\n",info.c_str()));
  output.append(Form("EnergyCoeff:  "));
  for(auto &i : energy_coeff) { output.append(Form("\t%.04f",i)); }
  output.append("\n");
  output.append("EfficienyCoeff:");
  for(auto &i : efficiency_coeff) { output.append(Form("\t%.04f",i)); }
  output.append("\n}\n-----------------------------------\n");
  return output;
}

void TChannel::Copy(TObject &rhs) const {
  TNamed::Copy(rhs);
  ((TChannel&)rhs).address = address;
  ((TChannel&)rhs).number  = number;
  ((TChannel&)rhs).info    = info;
  ((TChannel&)rhs).energy_coeff = energy_coeff;
  ((TChannel&)rhs).efficiency_coeff = efficiency_coeff;
  return;
}

void TChannel::Clear(Option_t *opt) {
  TNamed::Clear(opt);
  address         = 0xffffffff;
  number          = -1;
  info.clear();
  energy_coeff.clear();
  efficiency_coeff.clear();
  fMnemonic.Clear();
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
  if(fChannelMap.count(add)==1) {
    chan = fChannelMap.at(add);
  }
  return chan;
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
  if(this->GetEnergyCoeff().size()>0)
     oldchan->SetEnergyCoeff(this->GetEnergyCoeff());
  if(this->GetEfficiencyCoeff().size()>0)
     oldchan->SetEfficiencyCoeff(this->GetEfficiencyCoeff());
  return true;
}

bool TChannel::ReplaceChannel(TChannel *oldchan)  {
  this->Copy(*oldchan);
}

bool TChannel::RemoveChannel(TChannel &chan) {
  if(fChannelMap.count(chan.GetAddress()==1)) {
    fChannelMap.erase(chan.GetAddress());
    return true;
  }
  return false;
}

int TChannel::DeleteAllChannels()  {
  int count;
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
   //Removes the the string "trimCars" from  the string 'line'
  if(line->length() == 0)
    return;
  std::size_t found = line->find_first_not_of(trimChars);
  if(found != std::string::npos)
    *line = line->substr(found, line->length());
  found = line->find_last_not_of(trimChars);
  if(found != std::string::npos)
    *line = line->substr(0, found + 1);
  return;
}


void TChannel::DestroyCalibrations() {
  DestroyEnergyCoeff();
  DestroyEfficiencyCoeff();
  return;
};

double TChannel::CalEnergy(int charge) {
  if(charge==0)
    return 0.000;
  double dcharge = (double)charge + gRandom->Uniform();
  return CalEnergy(dcharge);
}

double TChannel::CalEnergy(double charge) {
  if(energy_coeff.size()==0)
     return charge;
  double cal_chg = 0.000;
  // Evaluate the polynomial
  for(int i=energy_coeff.size()-1; i>=0; i--){
    double coef = energy_coeff[i];
    cal_chg *= charge;
    cal_chg += coef;
  }
  return cal_chg;
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
  char buffer[(int)length];
  infile.seekg(0,std::ios::beg);
  infile.read(buffer,(int)length);
  sbuffer.assign(buffer);

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
    ofstream calout;
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
}


int TChannel::ParseInputData(std::string &input,Option_t *opt) {
  std::istringstream infile(input);
  TChannel *channel = 0;
  std::string line;
  int linenumber = 0;
  int newchannels = 0;

  bool creatednewchannel = false;
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
      colon      == std::string::npos)
      continue;
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
        std::string type = line.substr(0,colon);
        line = line.substr(colon+1,line.length());
        trim(&line);
        std::istringstream ss(line);
        int j=0;
        while(type[j]) {
           char  c = *(type.c_str()+j);
           c = toupper(c);
           type[j++] = c;
        }
        if(type.compare("NAME")==0) {
          channel->SetName(line.c_str());
        } else if(type.compare("ADDRESS")==0) {
          unsigned int add = 0; ss >> add;
          if(add==0) {
            std::stringstream newss;
            newss << std::hex << line;
            newss >> add;
          }
          channel->SetAddress(add);
        } else if(type.compare("NUMBER")==0) {
          int num; ss>>num;
          channel->SetNumber(num);
        } else if(type.compare("INFO")==0) {
          channel->SetInfo(line.c_str());
        } else if((type.compare("ENERGYCOEFF")==0) ||
                  (type.compare("ENGCOEFF")==0)) {
          channel->DestroyEnergyCoeff();
          double value;
          while(ss>>value) { channel->AddEnergyCoeff(value); }
        } else if((type.compare("EFFICIENCEYCOEFF")==0) ||
                  (type.compare("EFFCOEFF")==0)) {
          channel->DestroyEfficiencyCoeff();
          double value;
          while(ss>>value) { channel->AddEfficiencyCoeff(value); }
        }
      }
    }
  }
  if(!strcmp(opt,"debug"))
     printf("parsed %i lines,\n",linenumber);
  return newchannels;
};


void TChannel::Streamer(TBuffer &R__b) {
  this->SetBit(kCanDelete);
  UInt_t R__s, R__c;
  if(R__b.IsReading()) {
     Version_t R__v = R__b.ReadVersion(&R__s,&R__c);
     TNamed::Streamer(R__b);
     if(R__v>1) { }
     { TString R__str; R__str.Streamer(R__b); fChannelData.assign(R__str.Data()); }
     ParseInputData(fChannelData,"debug");
     R__b.CheckByteCount(R__s,R__c,TChannel::IsA());
  } else {
     R__c = R__b.WriteVersion(TChannel::IsA(),true);
     TNamed::Streamer(R__b);
     { TString R__str = fChannelData.c_str(); R__str.Streamer(R__b);}
     R__b.SetByteCount(R__c,true);
  }
}

void TChannel::MNEMONIC::Unpack(std::string name) {
  if(name.length()<9) {
    return;
  }
  std::string buf;

  system.assign(name,0,3);

  buf.clear(); buf.assign(name,3,2);
  array_position = (uint16_t)atoi(buf.c_str());
  arraysubposition.assign(name,5,1);
  collectedcharge.assign(name,6,1);
  buf.clear(); buf.assign(name,7,2);
  segment = (uint16_t)atoi(buf.c_str());
  return;
}

void TChannel::MNEMONIC::Clear(Option_t* opt){
  array_position = -1;
  segment = -1;
  system = "";
  arraysubposition = "";
  collectedcharge = "";
}
