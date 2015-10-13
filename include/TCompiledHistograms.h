#ifndef _TCOMPILEDHISTOGRAMS_H_
#define _TCOMPILEDHISTOGRAMS_H_

#include <memory>

#include "TObject.h"

#include "DynamicLibrary.h"
#include "TRuntimeObjects.h"

class TCompiledHistograms : public TObject {
public:
  TCompiledHistograms();
  TCompiledHistograms(std::string libname);

  void Load(std::string libname);
  void Call(TRuntimeObjects& obj);
  void Reload();

private:
  void swap(TCompiledHistograms& other);
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

  ClassDef(TCompiledHistograms, 0);
};

#endif /* _TCOMPILEDHISTOGRAMS_H_ */
