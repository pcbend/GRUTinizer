#include "TCompiledHistograms.h"

#include <algorithm>
#include <fstream>

#include <sys/stat.h>

typedef void* __attribute__((__may_alias__)) void_alias;

TCompiledHistograms::TCompiledHistograms()
  : libname(""), library(nullptr), func(nullptr),
    last_modified(0), last_checked(0), check_every(5) { }

TCompiledHistograms::TCompiledHistograms(std::string libname)
  : libname(libname), check_every(5) {
  library = std::make_shared<DynamicLibrary>(libname.c_str(), true);
  // Casting required to keep gcc from complaining.
  *(void_alias*)(&func) = library->GetSymbol("MakeHistograms");
  last_modified = get_timestamp();
  last_checked = time(NULL);
}

time_t TCompiledHistograms::get_timestamp() {
  struct stat buf;
  stat(libname.c_str(), &buf);
  return buf.st_mtime;
}

bool TCompiledHistograms::file_exists() {
  return std::ifstream(libname);
}

void TCompiledHistograms::Load(std::string libname) {
  TCompiledHistograms other(libname);
  swap(other);
}

void TCompiledHistograms::Reload() {
  if (file_exists() &&
      get_timestamp() > last_modified) {
    TCompiledHistograms other(libname);
    swap(other);
  }
  last_checked = time(NULL);
}

void TCompiledHistograms::swap(TCompiledHistograms& other) {
  std::swap(libname, other.libname);
  std::swap(library, other.library);
  std::swap(func, other.func);
  std::swap(last_modified, other.last_modified);
  std::swap(last_checked, other.last_checked);
  std::swap(check_every, other.check_every);
}

void TCompiledHistograms::Call(TRuntimeObjects& obj) {
  if(!library){
    return;
  }

  if(time(NULL) > last_checked + check_every){
    Reload();
  }
  func(obj);
}
