#include "TGRUTOptions.h"

#include <algorithm>
#include <cctype>
#include <iostream>

#include "TEnv.h"

#include "ArgParser.h"
#include "DynamicLibrary.h"
#include "TGRUTUtilities.h"
#include "GRootCommands.h"

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
  input_val_files.clear();
  input_macro_files.clear();

  input_ring = "";

  options_file.clear();

  fDefaultFileType = kFileType::UNKNOWN_FILETYPE;

  fExtractWaves     = false;
  fExitAfterSorting = false;
  fHelp       = false;
  fShowedVersion = false;
  fShowLogo   = true;
  fSortRaw    = true;
  fSortRoot   = false;
  fIsOnline   = false;
  fMakeHistos = false;
  fSortMultiple = false;

  fShouldExit = false;
}

void TGRUTOptions::Print(Option_t* opt) const { }

void TGRUTOptions::Load(int argc, char** argv) {
  Clear();
  detector_environment = std::string(getenv("GRUTSYS")) + "/config/DetectorEnvironment.env";
  compiled_histogram_file = gEnv->GetValue("GRUT.HistLib","");
  if(compiled_histogram_file.length() == 0){
    compiled_histogram_file = std::string(getenv("GRUTSYS")) + "/libraries/libMakeHistos.so";
  }

  s800_inverse_map_file = "";//"invmap.inv";

  // Load default TChannels, if specified.
  {
    std::string default_calfile = gEnv->GetValue("GRUT.CalFile","");
    if(default_calfile.length()){
      input_cal_files.push_back(default_calfile);
    }
  }

  // Load default GValues, if specified.
  {
    std::string default_valfile = gEnv->GetValue("GRUT.ValFile","");
    if(default_valfile.length()){
      input_val_files.push_back(default_valfile);
    }
  }

  ArgParser parser;

  std::vector<std::string> input_files;
  std::string default_file_format;

  //parser.option() will initialize boolean values to false.

  parser.default_option(&input_files)
    .description("Input file(s)");
  parser.option("o output", &output_file)
    .description("Root output file");
  parser.option("f filter-output",&output_filtered_file)
    .description("Output file for raw filtered data");
  parser.option("hist-output",&output_histogram_file)
    .description("Output file for histograms");
  parser.option("r ring",&input_ring)
    .description("Input ring source (host/ringname).  Requires --format to be specified.");
  parser.option("l no-logo", &fShowLogo)
    .description("Inhibit the startup logo")
    .default_value(true);
  parser.option("H histos", &fMakeHistos)
    .description("attempt to run events through MakeHisto lib.")
    .default_value(false);
  parser.option("n no-sort", &fSortRaw)
    .description("Load raw data files without sorting")
    .default_value(true);
  parser.option("m sort-multiple", &fSortMultiple)
    .description("If passed multiple raw data files, treat them as one file.")
    .default_value(false);
  parser.option("s sort", &fSortRoot)
    .description("Attempt to loop through root files.")
    .default_value(false);
  parser.option("t time-sort", &fTimeSortInput)
    .description("Reorder raw events by time");
  parser.option("time-sort-depth",&fTimeSortDepth)
    .description("Number of events to hold when time sorting")
    .default_value(100000);
  parser.option("build-window", &fBuildWindow)
    .description("Build window, timestamp units")
    .default_value(1000);
  parser.option("long-file-description", &fLongFileDescription)
    .description("Show full path to file in status messages")
    .default_value(false);
  parser.option("format",&default_file_format)
    .description("File format of raw data.  Allowed options are \"EVT\" and \"GEB\"."
                 "If unspecified, will be guessed from the filename.");
  parser.option("g start-gui",&fStartGui)
    .description("Start the GUI")
    .default_value(false);
  parser.option("w gretina-waves",&fExtractWaves)
    .description("Extract wave forms to data class when available.")
    .default_value(false);
  parser.option("q quit", &fExitAfterSorting)
    .description("Run in batch mode");
  parser.option("h help ?", &fHelp)
    .description("Show this help message");
  parser.option("v version", &fShowedVersion)
    .description("Show version information");


  // Look for any arguments ending with .info, pass to parser.
  for(int i=0; i<argc; i++){
    std::string filename = argv[i];
    if(DetermineFileType(filename) == kFileType::CONFIG_FILE){
      try{
        parser.parse_file(filename);
      } catch (ParseError& e){
        std::cerr << "ERROR: " << e.what() << "\n"
                  << parser << std::endl;
        fShouldExit = true;
      }
    }
  }

  // Look at the command line.
  try{
    parser.parse(argc, argv);
  } catch (ParseError& e){
    std::cerr << "ERROR: " << e.what() << "\n"
              << parser << std::endl;
    fShouldExit = true;
  }

  // Print help if requested.
  if(fHelp){
    Version();
    std::cout << parser << std::endl;
    fShouldExit = true;
  }

  // Print version if requested
  if(fShowedVersion) {
    Version();
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

  if(output_histogram_file.length()>0 &&
     output_histogram_file != "none") {
    fMakeHistos = true;
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
  } else if ((ext == "c") || (ext == "C")
             || (ext == "c+") || (ext == "C+")
             || (ext == "c++") || (ext == "C++")) {
    return kFileType::ROOT_MACRO;
  } else if (ext == "dat" || ext == "cvt") {
    if(filename.find("GlobalRaw")!=std::string::npos)
      return kFileType::GRETINA_MODE3;
    return kFileType::GRETINA_MODE2;
  } else if (ext == "env") {
    return kFileType::DETECTOR_ENVIRONMENT;
  } else if (ext == "hist") {
    return kFileType::GUI_HIST_FILE;
  } else if (ext == "so") {
    DynamicLibrary lib(filename);
    if(lib.GetSymbol("MakeHistograms")) {
      return kFileType::COMPILED_HISTOGRAMS;
    } else if (lib.GetSymbol("FilterCondition")) {
      return kFileType::COMPILED_FILTER;
    } else {
      std::cerr << filename << " did not contain MakeHistograms() or FilterCondition()" << std::endl;
      return kFileType::UNKNOWN_FILETYPE;
    }
  } else if (ext == "info") {
    return kFileType::CONFIG_FILE;
  } else if (ext == "inv") {
    return kFileType::S800_INVMAP;
  } else if (ext == "val"){
    return kFileType::GVALUE;
  } else if (ext == "win"){
    return kFileType::PRESETWINDOW;
  } else if (ext == "cuts") {
    return kFileType::CUTS_FILE;
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

    case kFileType::GUI_HIST_FILE:
      input_gui_hist_files.push_back(filename);
      return true;

    case kFileType::DETECTOR_ENVIRONMENT:
      detector_environment = filename;
      return true;

    case kFileType::COMPILED_HISTOGRAMS:
      compiled_histogram_file = filename;
      return true;

    case kFileType::COMPILED_FILTER:
      compiled_filter_file = filename;
      return true;

    case kFileType::S800_INVMAP:
      s800_inverse_map_file = filename;
      return true;

    case kFileType::GVALUE:
      input_val_files.push_back(filename);
      return true;

    case kFileType::PRESETWINDOW:
      input_win_files.push_back(filename);
      return true;

    case kFileType::CUTS_FILE:
      input_cuts_files.push_back(filename);
      return true;

    case kFileType::CONFIG_FILE:
      return false;
    case kFileType::UNKNOWN_FILETYPE:
    default:
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
