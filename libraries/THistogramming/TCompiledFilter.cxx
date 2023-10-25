#include "TCompiledFilter.h"

#include <algorithm>
#include <fstream>
#include <iostream>

#include <sys/stat.h>

#include "TH1.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TObject.h"
#include "TROOT.h"
#include "TKey.h"

#include "GValue.h"
#include "GRootCommands.h"
#include "TPreserveGDirectory.h"

typedef void* __attribute__((__may_alias__)) void_alias;

TCompiledFilter::TCompiledFilter()
  : libname(""), library(nullptr), func(nullptr),
    last_modified(0), last_checked(0), check_every(5),
    obj(&objects, &gates, cut_files) { }

TCompiledFilter::TCompiledFilter(std::string input_lib)
  : TCompiledFilter() {

  libname = input_lib;
  library = std::make_shared<DynamicLibrary>(libname.c_str(), true);

  // Casting required to keep gcc from complaining.
  *(void_alias*)(&func) = library->GetSymbol("FilterCondition");

  if(!func){
    std::cout << "Could not find FilterCondition() inside "
              <<"\"" << input_lib << "\"" << std::endl;
  }
  last_modified = get_timestamp();
  last_checked = time(NULL);
}

time_t TCompiledFilter::get_timestamp() {
  struct stat buf;
  stat(libname.c_str(), &buf);
  return buf.st_mtime;
}

bool TCompiledFilter::file_exists() {
  std::ifstream infile(libname);
  return infile.is_open();
}

void TCompiledFilter::Load(std::string libname) {
  TCompiledFilter other(libname);
  swap_lib(other);
}

void TCompiledFilter::Reload() {
  if (file_exists() &&
      get_timestamp() > last_modified) {
    TCompiledFilter other(libname);
    swap_lib(other);
  }
  last_checked = time(NULL);
}

void TCompiledFilter::swap_lib(TCompiledFilter& other) {
  std::swap(libname, other.libname);
  std::swap(library, other.library);
  std::swap(func, other.func);
  std::swap(last_modified, other.last_modified);
  std::swap(last_checked, other.last_checked);
  std::swap(check_every, other.check_every);
}

bool TCompiledFilter::MatchesCondition(TUnpackedEvent& detectors) {
  if(time(NULL) > last_checked + check_every){
    Reload();
  }

  if(!library || !func){
    return true;
  }

  obj.SetDetectors(&detectors);
  return func(obj);
}

void TCompiledFilter::AddCutFile(TFile* cut_file) {
  if(cut_file) {
    cut_files.push_back(cut_file);
    TIter iter(cut_file->GetListOfKeys());
    while (TKey *key = (TKey*)iter.Next()){
      TObject *obj = key->ReadObj();
      if (obj->InheritsFrom("TCutG")){
        gates.Add(obj);
      }
    }
  }
}
