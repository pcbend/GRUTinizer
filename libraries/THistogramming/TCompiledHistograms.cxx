#include "TCompiledHistograms.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <cctype>

#include <sys/stat.h>

#include "TH1.h"
#include "TFile.h"
#include "TDirectory.h"
#include "THttpServer.h"
#include "TObject.h"
#include "TROOT.h"
#include "TKey.h"
#include "TString.h"

#include "GValue.h"
#include "TPreserveGDirectory.h"

typedef void* __attribute__((__may_alias__)) void_alias;

namespace {
std::string BuildHttpEngineString(const std::string& input) {
  bool is_port = !input.empty();
  for(char c : input) {
    if(!std::isdigit(static_cast<unsigned char>(c))) {
      is_port = false;
      break;
    }
  }
  if(is_port) {
    return "http:" + input + "?top=GRUTinizer";
  }
  if(input.find('?') == std::string::npos) {
    return input + "?top=GRUTinizer";
  }
  return input;
}
}

TCompiledHistograms::TCompiledHistograms()
  : libname(""), library(nullptr), func(nullptr),
    last_modified(0), last_checked(0), check_every(5),
    http_publish_interval(1), last_http_publish(0),
    default_directory(0), http_server(nullptr),
    obj(&objects, &gates, cut_files) { }

TCompiledHistograms::TCompiledHistograms(std::string input_lib)
  : TCompiledHistograms() {

  libname = input_lib;
  library = std::make_shared<DynamicLibrary>(libname.c_str(), true);
  // Casting required to keep gcc from complaining.
  *(void_alias*)(&func) = library->GetSymbol("MakeHistograms");

  if(!func){
    std::cout << "Could not find MakeHistograms() inside "
              <<"\"" << input_lib << "\"" << std::endl;
  }
  last_modified = get_timestamp();
  last_checked = time(NULL);
}

TCompiledHistograms::~TCompiledHistograms() {
  delete http_server;
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
    else if(obj->InheritsFrom(TDirectory::Class())){
      TDirectory* dir = (TDirectory*)obj;
      TIter dirnext(dir->GetList());
      TObject* dirobj;
      while((dirobj=dirnext())){
	if(dirobj->InheritsFrom(TH1::Class())){
	  TH1* hist = (TH1*)dirobj;
	  hist->Reset();
	}
      }
    }

  }
  std::cout << "ended " << std::endl;
}

time_t TCompiledHistograms::get_timestamp() {
  struct stat buf;
  stat(libname.c_str(), &buf);
  return buf.st_mtime;
}

bool TCompiledHistograms::file_exists() {
  std::ifstream infile(libname);
  return infile.is_open();
}

void TCompiledHistograms::Write() {
  objects.Sort();

  TIter next(&objects);
  TObject *obj;
  while((obj=next())){
    if(obj->InheritsFrom(TDirectory::Class())){
      TPreserveGDirectory preserve;
      TDirectory *dir = (TDirectory*)obj;
      gDirectory->mkdir(dir->GetName())->cd();
      TIter dir_next(dir->GetList());
      TObject *dir_obj;
      while((dir_obj=dir_next())){
	dir_obj->Write();
      }
    } else {
      obj->Write();
    }
  }



  //  objects.Write();
  //TPreserveGDirectory preserve;
  //gDirectory->mkdir("variables")->cd();
  //variables.Write();
}

void TCompiledHistograms::EnableLiveHttp(const std::string& server) {
  if(server.empty() || server == "none" || server == "off") {
    return;
  }
  if(http_server) {
    return;
  }

  std::string engine = BuildHttpEngineString(server);
  http_server = new THttpServer(engine.c_str());
  http_server->SetReadOnly(kTRUE);
  http_server->CreateServerThread();
  std::cout << "Publishing live histograms with ROOT THttpServer at "
            << engine << std::endl;
}

void TCompiledHistograms::RegisterLiveHttpObject(const char* folder, TObject* obj) {
  if(!http_server || !obj) {
    return;
  }
  if(http_registered_objects.count(obj)) {
    return;
  }
  if(http_server->Register(folder, obj)) {
    http_registered_objects.insert(obj);
  }
}

void TCompiledHistograms::PublishLiveHttp(bool force) {
  if(!http_server) {
    return;
  }

  time_t now = time(NULL);
  if(!force && now <= last_http_publish + http_publish_interval) {
    return;
  }
  last_http_publish = now;

  TIter next(&objects);
  TObject* current;
  while((current = next())) {
    TDirectory* dir = dynamic_cast<TDirectory*>(current);
    if(dir) {
      TString folder = TString::Format("/histograms/%s", dir->GetName());
      TIter dir_next(dir->GetList());
      TObject* dir_object;
      while((dir_object = dir_next())) {
        RegisterLiveHttpObject(folder.Data(), dir_object);
      }
    } else {
      RegisterLiveHttpObject("/histograms", current);
    }
  }
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

  if(!library || !func || !default_directory){
    return;
  }

  TPreserveGDirectory preserve;
  default_directory->cd();

  obj.SetDetectors(&detectors);
  func(obj);
  PublishLiveHttp();
}

void TCompiledHistograms::AddCutFile(TFile* cut_file) {
  if(cut_file) {
    cut_files.push_back(cut_file);
    TIter iter(cut_file->GetListOfKeys());
    while(TKey *key = (TKey*)iter.Next()) {
      TObject *obj = key->ReadObj();
      if(obj->InheritsFrom("TCutG"))
        gates.Add(obj);
    }

  }
}

void TCompiledHistograms::SetDefaultDirectory(TDirectory* dir) {
  default_directory = dir;

  TObject* obj = NULL;
  TIter next(&objects);
  while((obj = next())) {
    TH1* hist = dynamic_cast<TH1*>(obj);
    if(hist) {
      hist->SetDirectory(dir);
    }
  }
  PublishLiveHttp(true);
}
