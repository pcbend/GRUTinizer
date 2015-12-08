#include "TGRUTUtilities.h"

#include <sys/stat.h>
#include <iostream>
#include <fstream>

#include <TString.h>
#include <TRegexp.h>
#include <TPRegexp.h>


bool file_exists(const char *filename){
  //std::ifstream(filename);
  struct stat buffer;  
  return (stat(filename,&buffer)==0);
}

std::string get_run_number(std::string input) {

  TPRegexp pnumber("([0-9]+-)?[0-9]+");

  TRegexp number("[0-9]");
  TString sinput = input.c_str();
  int i=0;
  std::string toreturn ="";
  
  while(true) {
    int result = sinput.Index(number,i);
    if(result == -1) {
      if(i==0)
        i=-1;
      break;
    }
    i = result+1;
  }
  i-=1;
  //std::cout << "Input string: \"" << input << "\"" << std::endl;
  //std::cout << "Found last numeral at " << i << std::endl;
  //std::cout << "Last numeral is '" << sinput[i] << "'" << std::endl;
  if(i==-1)
    return toreturn;
  int end = i;
  for(i=end; i>0; i--){
    if(sinput.Index(pnumber,i) != i){
      i++;
      break;
    }
  }
  //std::cout << "Found run number start at " << i << std::endl;
  int begin = i;
  return input.substr(begin, end-begin+1);
  

  
}


