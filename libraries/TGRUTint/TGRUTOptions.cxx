#include "TGRUTOptions.h"

#include <algorithm>
#include <cctype>
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

  fDefaultFileType = kFileType::UNKNOWN_FILETYPE;

  fCommandServer    = true;
  fExtractWaves     = false;
  fExitAfterSorting = false;
  fHelp = false;
  fShowLogo = true;
  fSortRaw = true;

  fShouldExit = false;
}

void TGRUTOptions::Print(Option_t* opt) const { }

void TGRUTOptions::Load(int argc, char** argv) {
  Clear();
  detector_environment = std::string(getenv("GRUTSYS")) + "/config/DetectorEnvironment.env";

  ArgParser parser;

  std::vector<std::string> input_files;
  std::string default_file_format;

  //parser.option() will initialize boolean values to false.

  parser.default_option(&input_files)
    .description("Input file(s)");
  parser.option("o output", &output_file)
    .description("Root output file");
  parser.option("3 include-mode3 ", &fIgnoreMode3)
    .description("skip over any gretina mode3 data")
    .default_value(true);
  parser.option("r ring",&input_ring)
    .description("Input ring source (host/ringname).  Requires --format to be specified.");
  parser.option("n online",&fIsOnline)
    .description("Continually read the input file as it is written");
  parser.option("l no-logo", &fShowLogo)
    .description("Inhibit the startup logo")
    .default_value(true);
  parser.option("n no-sort", &fSortRaw)
    .description("Load raw data files without sorting")
    .default_value(true);
  parser.option("t time-sort", &fTimeSortInput)
    .description("Reorder raw events by time");
  parser.option("port", &fCommandPort)
    .description("Port on which to listen for commands")
    .default_value(9090);
  parser.option("f format",&default_file_format)
    .description("File format of raw data.  Allowed options are \"EVT\" and \"GEB\"."
                 "If unspecified, will be guessed from the filename.");
  parser.option("w getwaves",&fExtractWaves)
    .description("Extract wave forms to data class when available.")
    .default_value(false);
  parser.option("no-backup",&fMakeBackupFile)
    .description("Disable the \"last.root\" backup file")
    .default_value(true);
  parser.option("noserver", &fCommandServer)
    .description("Start program without default command server running.")
    .default_value(true);
  parser.option("ignore-errors ignore_errors", &fIgnoreErrors)
    .description("Don't print warning messages.  Use at your own risk.");
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

  // Handle the default file format
  if(default_file_format.length()){
    // Make the string be lowercase
    std::transform(default_file_format.begin(), default_file_format.end(), default_file_format.begin(),
                   (int (*)(int))std::tolower);
    if(default_file_format == "geb"){
      fDefaultFileType = kFileType::GRETINA_MODE2;
    } else if (default_file_format == "evt") {
      fDefaultFileType = kFileType::NSCL_EVT;
    } else {
      std::cerr << "ERROR: Unknown file type: \"" << default_file_format << "\"\n"
                << parser << std::endl;
      fShouldExit = true;
    }
  }

  if(input_ring.length() && fDefaultFileType == kFileType::UNKNOWN_FILETYPE){
    std::cerr << "ERROR: Must specify --format when reading from a ring\n"
              << parser << std::endl;
    fShouldExit = true;
  }

  if(fExitAfterSorting){
    fMakeBackupFile = false;
  }

  for(auto& file : input_files){
    FileAutoDetect(file);
  }
}

kFileType TGRUTOptions::DetermineFileType(const std::string& filename) const{
  size_t dot_pos = filename.find_last_of('.');
  std::string ext = filename.substr(dot_pos+1);

  bool isZipped = (ext=="gz") || (ext=="bz2") || (ext=="zip");
  if(isZipped){
    std::string remaining = filename.substr(0,dot_pos);
    ext = remaining.substr(remaining.find_last_of('.')+1);
  }

  if(ext=="evt"){
    return kFileType::NSCL_EVT;
  } else if (ext == "cal") {
    return kFileType::CALIBRATED;
  } else if (ext == "root") {
    return kFileType::ROOT_DATA;
  } else if ((ext == "c") || (ext == "C") || (ext == "c+") || (ext == "C+")) {
    return kFileType::ROOT_MACRO;
  } else if (ext == "dat" || ext == "cvt") {
    if(filename.find("GlobalRaw")!=std::string::npos)
      return kFileType::GRETINA_MODE3;
    return kFileType::GRETINA_MODE2;
  } else if (ext == "env") {
    return kFileType::DETECTOR_ENVIRONMENT;
  } else {
    return kFileType::UNKNOWN_FILETYPE;
  }
}

bool TGRUTOptions::FileAutoDetect(const std::string& filename) {
  switch(DetermineFileType(filename)){
  case kFileType::NSCL_EVT:
  case kFileType::GRETINA_MODE2:
  case kFileType::GRETINA_MODE3:
    input_raw_files.push_back(filename);
    return true;

  case kFileType::ROOT_DATA:
    input_root_files.push_back(filename);
    return true;

  case kFileType::ROOT_MACRO:
    input_macro_files.push_back(filename);
    return true;

  case kFileType::CALIBRATED:
    input_cal_files.push_back(filename);
    return true;

  case kFileType::DETECTOR_ENVIRONMENT:
    detector_environment = filename;
    return true;

  case kFileType::UNKNOWN_FILETYPE:
    printf("\tDiscarding unknown file: %s\n",filename.c_str());
    return false;
  }
}

std::string TGRUTOptions::GenerateOutputFilename(const std::string& filename){
  return "temp.root";
}

std::string TGRUTOptions::GenerateOutputFilename(const std::vector<std::string>& filename){
  return "temp_from_multi.root";
}
