#include "TCompiledHistograms.h"

#include <algorithm>
#include <fstream>
#include <iostream>

#include <sys/stat.h>

#include "TH1.h"

#include "FullPath.h"
#include "GValue.h"
#include "TPreserveGDirectory.h"

typedef void* __attribute__((__may_alias__)) void_alias;

TCompiledHistograms::TCompiledHistograms()
  : libname(""), library(nullptr), func(nullptr),
    last_modified(0), last_checked(0), check_every(5),
    default_directory(0) { }

TCompiledHistograms::TCompiledHistograms(std::string libname)
  : libname(full_path(libname)), check_every(5),
    default_directory(0) {
  library = std::make_shared<DynamicLibrary>(this->libname.c_str(), true);
  // Casting required to keep gcc from complaining.
  *(void_alias*)(&func) = library->GetSymbol("MakeHistograms");
  last_modified = get_timestamp();
  last_checked = time(NULL);
}

void TCompiledHistograms::ClearHistograms() {
  std::lock_guard<std::mutex> lock(mutex);

  TIter next(&objects);
  TObject* obj;
  while((obj = next())){
    if(obj->InheritsFrom(TH1::Class())){
      TH1* hist = (TH1*)obj;
      hist->Reset();
    }
  }
}

time_t TCompiledHistograms::get_timestamp() {
  struct stat buf;
  stat(libname.c_str(), &buf);
  return buf.st_mtime;
}

bool TCompiledHistograms::file_exists() {
  return std::ifstream(libname);
}

void TCompiledHistograms::Write() {
  objects.Write();
  TPreserveGDirectory preserve;
  gDirectory->mkdir("variables")->cd();
  variables.Write();
}

void TCompiledHistograms::Load(std::string libname) {
  TCompiledHistograms other(libname);
  swap_lib(other);
}

void TCompiledHistograms::Reload() {
  if (file_exists() &&
      get_timestamp() > last_modified) {
    TCompiledHistograms other(libname);
    swap_lib(other);
  }
  last_checked = time(NULL);
}

void TCompiledHistograms::swap_lib(TCompiledHistograms& other) {
  std::swap(libname, other.libname);
  std::swap(library, other.library);
  std::swap(func, other.func);
  std::swap(last_modified, other.last_modified);
  std::swap(last_checked, other.last_checked);
  std::swap(check_every, other.check_every);
}

void TCompiledHistograms::Fill(TUnpackedEvent& detectors) {
  std::lock_guard<std::mutex> lock(mutex);
  if(time(NULL) > last_checked + check_every){
    Reload();
  }

  if(!library || !func){
    return;
  }

  TRuntimeObjects obj(detectors, &objects, &variables);
  func(obj);
}

TList* TCompiledHistograms::GetVariables() {
  return &variables;
}

void TCompiledHistograms::SetReplaceVariable(const char* name, double value) {
  GValue* val = (GValue*)variables.FindObject(name);
  if(val){
    val->SetValue(value);
  } else {
    GValue* val = new GValue(name, value);
    variables.Add(val);
  }
}

void TCompiledHistograms::RemoveVariable(const char* name) {
  GValue* val = (GValue*)variables.FindObject(name);
  if(val){
    variables.Remove(val);
  }
}
