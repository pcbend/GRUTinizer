
#include <TPresetCanvas.h>

#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>

#include <TGRUTUtilities.h>
#include <GCanvas.h>

#include <TROOT.h>

ClassImp(TPresetPad) 


TPresetPad::TPresetPad(const char *name) : TNamed(name,name) {
  Clear();
}

TPresetPad::TPresetPad(const TPresetPad &pad) : TNamed(pad), 
            fPadNumber(-1),fRowNumber(-1),fColNumber(-1) {
  pad.Copy(*this);
}


TPresetPad::~TPresetPad() { }

void TPresetPad::Copy(TObject &obj) const {
  TNamed::Copy(obj);
  ((TPresetPad&)obj).fPadNumber  = fPadNumber;
  ((TPresetPad&)obj).fRowNumber  = fRowNumber;
  ((TPresetPad&)obj).fColNumber  = fColNumber;
  ((TPresetPad&)obj).fObjectName = fObjectName;
  ((TPresetPad&)obj).fOption     = fOption;
  ((TPresetPad&)obj).fColor      = fColor;
}


void TPresetPad::Print(Option_t *opt) const {
  printf("%s\t{\n",GetName());
  printf("\tRow:   \t%i\n",fRowNumber);
  printf("\tCol:   \t%i\n",fColNumber);
  printf("\tObject:\t%s\n",fObjectName.Data());
  printf("\tOption:\t%s\n",fOption.Data());
  printf("\tColor: \t%s\n",fColor.Data());
  printf("}\n\n");
}

std::string TPresetPad::PrintString(Option_t *opt) const {
  char buffer[256];
  std::string output;
  sprintf(buffer,"%s\t{\n",GetName());
  output.append(buffer);
  sprintf(buffer,"\tRow:   \t%i\n",fRowNumber);
  output.append(buffer);
  sprintf(buffer,"\tCol:   \t%i\n",fColNumber);
  output.append(buffer);
  sprintf(buffer,"\tObject:\t%s\n",fObjectName.Data());
  output.append(buffer);
  sprintf(buffer,"\tOption:\t%s\n",fOption.Data());
  output.append(buffer);
  sprintf(buffer,"\tColor: \t%s\n",fColor.Data());
  output.append(buffer);
  sprintf(buffer,"}\n\n");
  output.append(buffer);
  return output;
}

void TPresetPad::Clear(Option_t *opt) {
  fPadNumber = -1;
  fRowNumber = -1;
  fColNumber = -1;
  fObjectName.Clear();
  fOption.Clear();
  fColor.Clear();
}

void TPresetPad::Draw(Option_t *opt) {
  //printf("%s called\n",__PRETTY_FUNCTION__);
  TObject *obj = gROOT->FindObjectAny(this->GetObjName());
  if(!obj) {
    printf("could not find %s",this->GetObjName());
    return;
  }
  if(obj->InheritsFrom(TH1::Class()) && ((TH1*)obj)->GetDimension()==1) {
    if(FindColor(this->GetColor()))
      ((TH1*)obj)->SetLineColor(FindColor(this->GetColor()));
  }
  obj->Draw(GetOption());
}

void TPresetPad::Set(int MaxCol,int MaxRow) {
  fPadNumber = (fRowNumber-1)*MaxCol+fColNumber;
}

ClassImp(TPresetCanvas)

TPresetCanvas::TPresetCanvas() {
  Clear();
}

TPresetCanvas::~TPresetCanvas() { }

void TPresetCanvas::Print(Option_t *opt) const {
  printf("%s\n",__PRETTY_FUNCTION__);
  return;
}

std::string TPresetCanvas::PrintString(Option_t *opt) const {
  std::string output;
  for(auto &pad : fPadMap) {
    output.append(pad.second.PrintString());
    output.append("\n");
  }
  return output;
}


void TPresetCanvas::Clear(Option_t *opt) {
  TNamed::Clear(opt);
  fCols  = -1;
  fRows  = -1;
  fPadMap.clear(); 
}

void TPresetCanvas::Draw(Option_t *opt) { 
  if(fRows<0 || fCols<0)
    Set();
  if(fRows<0 || fCols<0) {
    fprintf(stderr,"%s: trying to draw negative pad number. %i %i\n",__PRETTY_FUNCTION__,fRows,fCols);
    return;
  }
  gROOT->MakeDefCanvas();
  GCanvas *c = (GCanvas*)gPad->GetCanvas();
  c->Divide(fRows,fCols);
  for(auto pad : fPadMap) {
    c->cd(pad.second.GetPadNumber());
    pad.second.Draw();
  }

}

void TPresetCanvas::AddPad(TPresetPad &pad) { 
  if(pad.GetRowNumber()>fRows)
    fRows=pad.GetRowNumber();
  if(pad.GetColNumber()>fCols)
    fCols=pad.GetColNumber();
  fPadMap.insert(std::make_pair(fPadMap.size(),TPresetPad(pad))); 
}

void TPresetCanvas::Set() {
  std::map<int,TPresetPad> sortedmap;
  for(auto pad : fPadMap) {
    pad.second.Set(GetMaxCol(),GetMaxRow());
    if(sortedmap.count(pad.second.GetPadNumber())) {
      //if here, we already have a pad with that number, lets assume this 
      //was done on purpose and over laythem.
      //pad.second.AppendOption("same");
      sortedmap.insert(std::make_pair(pad.second.GetPadNumber()+fPadMap.size(),TPresetPad(pad.second)));
    }
    sortedmap.insert(std::make_pair(pad.second.GetPadNumber(),TPresetPad(pad.second)));
  }
  fPadMap = sortedmap;
} 


void TPresetCanvas::trim(std::string * line, const std::string & trimChars) {
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

int TPresetCanvas::ReadWinFile(const char* filename,Option_t *opt) {
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

  int pads_found = ParseInputData(sbuffer,opt);
  //if(pads_found) {
  //}
  return pads_found;
}

int TPresetCanvas::WriteWinFile(std::string outfilename,Option_t *opt) {
  int count =0;
  if(outfilename.length()>0) {
    std::ofstream winout;
    winout.open(outfilename.c_str());
    for(auto &iter : fPadMap) {
      winout << iter.second.PrintString(opt);
      winout << std::endl;
    }
    winout << std::endl;
    winout.close();
    count ++;
  } else {
    for(auto &iter : fPadMap) {
      iter.second.Print(opt);
      count++;
    }
  }
  return count;
}

int TPresetCanvas::ParseInputData(std::string input,Option_t *opt) {
 
  std::istringstream infile(input);
  std::string line;
  int linenumber  = 0;
  int newpads     = 0;
  
  TPresetPad pad;

  bool brace_open = false;
  std::string name;

  Clear();

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
      //if(pad.GetPadNumber()>0) {
        //pad.Set(this);
        fPadMap.insert(std::make_pair(fPadMap.size(),TPresetPad(pad)));
        newpads++;
      //}
      pad.Clear();
      name.clear();
    }
    //=============================================//
    if(openbrace != std::string::npos) {
       brace_open = true;
       name = line.substr(0,openbrace).c_str();
       trim(&name);
       pad.SetName(name.c_str());
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
          pad.SetName(line.c_str());
        } else if(type.compare("ROW")==0) {
          int num; ss>>num;
          if(num>fRows)
            fRows=num;
          pad.SetRowNumber(num);
        } else if(type.compare("COL")==0) {
          int num; ss>>num;
          if(num>fCols)
            fCols=num;
          pad.SetColNumber(num);
        } else if(type.compare("HISTNAME")==0   ||
                  type.compare("HIST")==0       ||
                  type.compare("OBJECTNAME")==0 ||
                  type.compare("OBJECT")==0     ) {
          pad.SetObjName(line.c_str());
        } else if(type.compare("OPTION")==0     ||
                  type.compare("OPT")==0        ){
          pad.SetOption(line.c_str());
        } else if(type.compare("COLOR")==0     ||
                  type.compare("LINECOLOR")==0 ||
                  type.compare("LINE")==0        ){
          pad.SetColor(line.c_str());
        }
      }
    }
  }
  if(newpads) {
    //for(auto pad : fPadMap)
    //  pad.second.Set(this);
    Set();
  }

  if(!strcmp(opt,"debug"))
     printf("parsed %i lines,\n",linenumber);
  return newpads;
}


void TPresetCanvas::Streamer(TBuffer &R__b) {
  this->SetBit(kCanDelete);
  //printf("TChannel Streamer.\n");
  UInt_t R__s, R__c;
  if(R__b.IsReading()) {
     Version_t R__v = R__b.ReadVersion(&R__s,&R__c);
     TNamed::Streamer(R__b);
     if(R__v>1) { }
     std::string buffer;
     { TString R__str; R__str.Streamer(R__b); buffer.assign(R__str.Data()); }
     ParseInputData(buffer,"debug");
     R__b.CheckByteCount(R__s,R__c,TPresetCanvas::IsA());
  } else {
     R__c = R__b.WriteVersion(TPresetCanvas::IsA(),true);
     TNamed::Streamer(R__b);
     std::string buffer = PrintString("all");
     { TString R__str = buffer.c_str(); R__str.Streamer(R__b);}
     R__b.SetByteCount(R__c,true);
  }
}

