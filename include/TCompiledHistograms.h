#ifndef _TCOMPILEDHISTOGRAMS_H_
#define _TCOMPILEDHISTOGRAMS_H_

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
  void Fill();
  void Reload();

  void RegisterDetector(TDetector* det);

  std::string GetLibraryName() const { return libname; }

  TList* GetVariables();
  void SetReplaceVariable(const char* name, double value);
  void RemoveVariable(const char* name);

  TList* GetHistograms();
  void ClearHistograms();

private:
  void swap_lib(TCompiledHistograms& other);
  time_t get_timestamp();
  bool file_exists();

  std::string libname;
#ifndef __CINT__
  std::shared_ptr<DynamicLibrary> library;
#endif
  void (*func)(TRuntimeObjects&);
  time_t last_modified;
  time_t last_checked;

  int check_every;

  TList detectors;
  TList objects;
  TList variables;

  ClassDef(TCompiledHistograms, 0);
};

#endif /* _TCOMPILEDHISTOGRAMS_H_ */
