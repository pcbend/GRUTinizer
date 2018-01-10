#include "TGRUTUtilities.h"

#include <cstdlib>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "TObjArray.h"
#include "TObjString.h"
#include "TPRegexp.h"
#include "TString.h"

bool file_exists(const char *filename){
  //std::ifstream(filename);
  struct stat buffer;
  return (stat(filename,&buffer)==0);
}

bool all_files_exist(const std::vector<std::string>& filenames) {
  for(auto& filename : filenames) {
    if(!file_exists(filename.c_str())) {
      return false;
    }
  }
  return true;
}

std::string get_short_filename(std::string filename) {
  std::string run_num = get_run_number(filename);
  size_t position = filename.find_last_of("/",filename.find(run_num));
  if(position == std::string::npos) {
    return filename;
  } else {
    return filename.substr(position + 1);
  }
}

std::string get_run_number(std::string input) {
  TPRegexp re(
    "(" // Begin capturing group.  This will hold the final return value
       "([0-9]+(-|_))?" //One or more digits, followed by a dash or an underscore.  This section may be omitted.
       "[0-9]+" //Followed by one or more digits
    ")" // End capturing group.
    "[^0-9]*$" // With no other digits before the end of the filename
  );
  TObjArray* matches = re.MatchS(input.c_str());

  std::string output;
  //std::cout << " matches->GetEntriesFast() = " << matches->GetEntriesFast() << std::endl;
  //for(int x=0;x<matches->GetEntriesFast();x++)
  //  std::cout << x << "\t" << ((TObjString*)matches->At(x))->GetString() << std::endl;

  if(matches->GetEntriesFast() >= 2){
    // Return a std::vector<std::string> ?
    // No, that would be too simple, too type-safe, and too memory-safe for ROOT.
    output = ((TObjString*)matches->At(1))->GetString();
  }
  delete matches;

  return output;
}

EColor FindColor(std::string name) {
  return color_system_map[name];
}

void CalculateParameters() {
  std::string command =
    std::string("xterm -e python2 ") + getenv("GRUTSYS") + "/libraries/TGRUTUtil/very-important-file &";
  std::cout << command << std::endl;
  system(command.c_str());
}



std::vector<int> MakeVectorFromFile(const char *fname) {
  std::vector<int> ret_vec;
  std::string line(fname);
  if(!line.length())
    return ret_vec;
  std::ifstream infile;
  infile.open(fname);
  line.clear();
  while(getline(infile,line)) {
    std::stringstream ss(line);
    int x;
    ss >> x;
    ret_vec.push_back(x);
  }
  return ret_vec;
}

// From http://stackoverflow.com/a/24315631
std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
  size_t start_pos = 0;
  while((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
  }
  return str;
}
