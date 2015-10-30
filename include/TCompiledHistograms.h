#ifndef _TCOMPILEDHISTOGRAMS_H_
#define _TCOMPILEDHISTOGRAMS_H_

#ifndef __CINT__
#include <mutex>
#endif
#include <memory>
#include <string>

#include "TObject.h"

#include "DynamicLibrary.h"
#include "TDetector.h"
#include "TRuntimeObjects.h"

class TCompiledHistograms : public TObject {
public:
  TCompiledHistograms();
  TCompiledHistograms(std::string libname);

  void Load(std::string libname);
  void Fill(TUnpackedEvent& detectors);
  void Reload();

  std::string GetLibraryName() const { return libname; }

  TList* GetVariables();
  void SetReplaceVariable(const char* name, double value);
  void RemoveVariable(const char* name);

  void ClearHistograms();

  TList* GetObjects(){ return &objects; }

  void Write();

private:
  void swap_lib(TCompiledHistograms& other);
  time_t get_timestamp();
  bool file_exists();

  std::string libname;
#ifndef __CINT__
  std::shared_ptr<DynamicLibrary> library;
  std::mutex mutex;
#endif
  void (*func)(TRuntimeObjects&);
  time_t last_modified;
  time_t last_checked;

  int check_every;

  TList objects;
  TList variables;

  ClassDef(TCompiledHistograms, 0);
};

#endif /* _TCOMPILEDHISTOGRAMS_H_ */
