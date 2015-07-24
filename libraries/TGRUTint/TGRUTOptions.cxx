#include "TGRUTOptions.h"

#include <iostream>

#include "ArgParser.h"

TGRUTOptions* TGRUTOptions::Get(int argc, char** argv){
  static TGRUTOptions* item = NULL;
  if(!item){
    item = new TGRUTOptions(argc, argv);
  }
  return item;
}

TGRUTOptions::TGRUTOptions(int argc, char** argv)
  : fShouldExit(false) {
  Load(argc, argv);
}

void TGRUTOptions::Clear(Option_t* opt) {
  input_raw_files.clear();
  input_root_files.clear();
  input_cal_files.clear();
  input_macro_files.clear();

  input_ring = "";

  options_file.clear();

  fExitAfterSorting = false;
  fHelp = false;
  fNoLogo = false;
  fNoSort = false;

  fShouldExit = false;
}

void TGRUTOptions::Print(Option_t* opt) const { }

void TGRUTOptions::Load(int argc, char** argv) {
  ArgParser parser;

  std::vector<std::string> input_files;

  //parser.option() will initialize boolean values to false.

  parser.default_option(&input_files)
    .description("Input file(s)");
  parser.option("i input",&input_files)
    .description("Input file(s)");
  parser.option("r ring",&input_ring)
    .description("Input ring source (host/ringname)");
  parser.option("l no-logo", &fNoLogo)
    .description("Inhibit the startup logo");
  parser.option("n no-sort", &fNoSort)
    .description("Load raw data files without sorting");
  parser.option("q quit", &fExitAfterSorting)
    .description("Run in batch mode");
  parser.option("h help ?", &fHelp)
    .description("Show this help message");

  try{
    parser.parse(argc, argv);
  } catch (ParseError& e){
    std::cerr << "ERROR: " << e.what() << "\n"
              << parser << std::endl;
    fShouldExit = true;
  }

  if(fHelp){
    std::cout << parser << std::endl;
    fShouldExit = true;
  }

  for(auto& file : input_files){
    FileAutoDetect(file);
  }
}

bool TGRUTOptions::FileAutoDetect(const std::string& filename) {
  size_t dot_pos = filename.find_last_of('.');
  std::string ext = filename.substr(dot_pos+1);

  bool isZipped = (ext=="gz") || (ext=="bz2") || (ext=="zip");
  if(isZipped){
    std::string remaining = filename.substr(0,dot_pos-1);
    ext = remaining.substr(remaining.find_last_of('.'));
  }

  if(ext == "root") {
    input_root_files.push_back(filename);
  } else if(ext == "cal") {
    input_cal_files.push_back(filename);
  } else if((ext == "c") || (ext == "C") || (ext == "c+") || (ext == "C+")) {
    input_macro_files.push_back(filename);
  } else if((ext == "evt") || (ext == "dat") ){
    input_raw_files.push_back(filename);
  } else {
    printf("\tDiscarding unknown file: %s\n",filename.c_str());
    return false;
  }

  return true;
}
