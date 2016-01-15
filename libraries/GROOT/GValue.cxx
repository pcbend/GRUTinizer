#include "GValue.h"

#include <iostream> 
#include <vector>
#include <algorithm>
#include <utility>
#include <fstream>
#include <sstream>

//std::string GValue::fValueData
//std::map<unsigned int, GValue*> GValue::fValueMap;
std::vector<GValue*> GValue::fValueVector;

GValue::GValue()
  : fValue(0.00) { }

GValue::GValue(const char *name,double value)
  : TNamed(name,name), fValue(value) { }

GValue::GValue(const char *name)
  : TNamed(name,name), fValue(0.00) { }

GValue::GValue(const GValue &val)
  : TNamed(val) {
  val.Copy(*this);
}

void GValue::Copy(TObject &obj) const {
  TNamed::Copy(obj);
  ((GValue&)obj).fValue = fValue;
}

GValue* GValue::FindValue(std::string name){
  GValue* value = 0;
  if(name.length()==0)
     return value;
  for(auto &iter : fValueVector) {
     value = iter;
     if(!name.compare(value->GetName()))
        break;
     value = 0;
  }
  return value;
}

bool GValue::AppendValue(GValue *oldvalue) {
  if(strlen(this->GetName()))
     oldvalue->SetName(this->GetName());
  if(this->GetValue() != -1)
     oldvalue->SetValue(this->GetValue());
  if(strlen(this->GetInfo()))
     oldvalue->SetInfo(this->GetInfo());
  return true;
}

bool GValue::ReplaceValue(GValue *oldvalue){
  this->Copy(*oldvalue);
  return true;
}

bool GValue::AddValue(GValue* value,Option_t *opt) {
  if(!value)
     return false;
  TString option(opt);

  std::string temp_string = value->GetName();
  
  if(GValue::FindValue(value->GetName())) {
     if(option.Contains("overwrite",TString::kIgnoreCase)) {
       GValue *oldvalue = FindValue(value->GetName());
       value->ReplaceValue(oldvalue);
       return true;
     } else {
       fprintf(stderr,"%s: Trying to add a value that already exists!\n",__PRETTY_FUNCTION__);
       return false;
     }
  } else if(temp_string.compare("") == 0) { //default value, get rid of it and ignore;
     delete value;
     value = 0;
  } else {
    fValueVector.push_back(value);
  }
  return true;
}

int GValue::ReadCalFile(const char* filename,Option_t *opt) {
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

  int values_found = ParseInputData(sbuffer,opt);
//if(values_found) {
//  //fFileNames.push_back(std::string(filename);;
//  fValueData = sbuffer; //.push_back(std::string((const char*)buffer);
//}
  return values_found;
}

//Parses input file. Should be in the form:
//NAME {
//  Name : 
//  Value : 
//  Info  : 
//}
int GValue::ParseInputData(std::string &input,Option_t *opt) {
  std::istringstream infile(input);
  GValue *value = 0;
  std::string line;
  int linenumber = 0;
  int newvalues = 0;

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
      if(value) {
        //Check whether value is in vector. If it isn't add it.
        GValue *cur_value = FindValue(value->GetName());
        if(!cur_value) {
          AddValue(value);
          newvalues++;
        } else {
          value->AppendValue(cur_value);
          delete value;
          newvalues++;
        }
      }
      value =0;
      name.clear();
    }
    //=============================================//
    if(openbrace != std::string::npos) {
       brace_open = true;
       name = line.substr(0,openbrace).c_str();
       trim(&name);
       value = new GValue(name.c_str());
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
          value->SetName(line.c_str());
        } else if(type.compare("VALUE")==0) {
          double num; ss>>num;
          value->SetValue(num);
        } else if(type.compare("INFO")==0) {
          value->SetInfo(line.c_str());
        }       
      }
    }
  }
  if(!strcmp(opt,"debug"))
     printf("parsed %i lines,\n",linenumber);
  return newvalues;
}

void GValue::trim(std::string * line, const std::string & trimChars) {
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

