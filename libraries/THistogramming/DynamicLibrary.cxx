#include "DynamicLibrary.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

#include <dlfcn.h>
#include <unistd.h>

//#include "RuntimeExceptions.h"
//#include "FullPath.h"

namespace {
  int incremental_id() {
    static std::mutex mutex;
    static int count = 0;
    std::lock_guard<std::mutex> lock(mutex);
    return count++;
  }
}

DynamicLibrary::DynamicLibrary(std::string libname, bool unique_name) {
  if(unique_name){
    std::stringstream ss;
    ss << "./temp_dynlib_" << getpid() << "_" << incremental_id() << ".so";
    std::string tempname = ss.str();
    //libname = full_path(libname);
    libname = libname;

    int error = symlink(libname.c_str(), tempname.c_str());
    if(error){
      return;
      //throw RuntimeSymlinkCreation("Could not make temp symlink");
    }
    library = dlopen(tempname.c_str(), RTLD_NOW);
    unlink(tempname.c_str());
  } else {
    library = dlopen(libname.c_str(), RTLD_NOW);
  }

  if(!library){
    return;
    //throw RuntimeFileNotFound(dlerror());
  }
}

DynamicLibrary::~DynamicLibrary() {
  if(library) {
    dlclose(library);
  }
}

DynamicLibrary::DynamicLibrary(DynamicLibrary&& other)
  : library(NULL){
  swap(other);
}

DynamicLibrary& DynamicLibrary::operator=(DynamicLibrary&& other){
  swap(other);
  return *this;
}

void DynamicLibrary::swap(DynamicLibrary& other){
  std::swap(library, other.library);
}

void* DynamicLibrary::GetSymbol(const char* symbol) {
  return dlsym(library, symbol);
}
