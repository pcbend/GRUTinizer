#ifndef _TCOMPILEDFILTER_H_
#define _TCOMPILEDFILTER_H_

#include <memory>
#include <string>

#include "TObject.h"
#include "TList.h"

#include "DynamicLibrary.h"
#include "TRuntimeObjects.h"

#include "TUnpackedEvent.h"

class TFile;

class TCompiledFilter : public TObject {
public:
  TCompiledFilter();
  TCompiledFilter(std::string libname);

  bool MatchesCondition(TUnpackedEvent& event);
  void Load(std::string libname);
  void Reload();

  std::string GetLibraryName() const { return libname; }

  void AddCutFile(TFile* cut_file);

private:
  void swap_lib(TCompiledFilter& other);
  time_t get_timestamp();
  bool file_exists();

  std::string libname;
#ifndef __CINT__
  std::shared_ptr<DynamicLibrary> library;
#endif
  bool (*func)(TRuntimeObjects&);
  time_t last_modified;
  time_t last_checked;

  int check_every;

  TList objects;
  TList gates;
  TRuntimeObjects obj;

  std::vector<TFile*> cut_files;

  ClassDef(TCompiledFilter, 0);
};

#endif /* _TCOMPILEDFILTER_H_ */
