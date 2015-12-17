#include "TGRUTUtilities.h"

#include <sys/stat.h>
#include <iostream>
#include <fstream>

#include "TObjArray.h"
#include "TObjString.h"
#include "TPRegexp.h"
#include "TString.h"


bool file_exists(const char *filename){
  //std::ifstream(filename);
  struct stat buffer;
  return (stat(filename,&buffer)==0);
}

std::string get_run_number(std::string input) {
  TPRegexp re(
    "(" // Begin capturing group.  This will hold the final return value
       "([0-9]+-)?" //One or more digits, followed by a dash.  This section may not be present.
       "[0-9]+" //Followed by one or more digits
    ")" // End capturing group.
    "[^0-9]*$" // With no other digits before the end of the filename
  );
  TObjArray* matches = re.MatchS(input.c_str());

  std::string output;
  if(matches->GetEntriesFast() >= 2){
    // Return a std::vector<std::string> ?
    // No, that would be too simple, too type-safe, and too memory-safe for ROOT.
    output = ((TObjString*)matches->At(1))->GetString();
  }
  delete matches;

  return output;
}
