#ifndef _TCOMPILEDHISTOGRAMS_H_
#define _TCOMPILEDHISTOGRAMS_H_

#ifndef __CINT__
#include <mutex>
#endif
#include <memory>
#include <set>
#include <string>

#include "TObject.h"
#include "TList.h"

#include "DynamicLibrary.h"
#include "TRuntimeObjects.h"

#include "TUnpackedEvent.h"

class TFile;
class THttpServer;
class TLiveHistogramWSHandler;

class TCompiledHistograms : public TObject {
public:
  using TObject::Write;

  TCompiledHistograms();
  TCompiledHistograms(std::string libname);
  ~TCompiledHistograms();

  void Load(std::string libname);
  void Fill(TUnpackedEvent& detectors);
  void Reload();

  std::string GetLibraryName() const { return libname; }

  void SetDefaultDirectory(TDirectory* dir);
  TDirectory* GetDefaultDirectory() { return default_directory; }

  void ClearHistograms();

  TList* GetObjects()   { return &objects;   }
  TList* GetGates()     { return &gates;     }

  void AddCutFile(TFile* cut_file);

  Int_t Write(const char* name = nullptr, Int_t option = 0, Int_t bufsize = 0);

  void EnableLiveHttp(const std::string& server);
  void PublishLiveHttp(bool force=false);


private:
  void RegisterLiveHttpObject(const char* folder, TObject* obj);
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
  int http_publish_interval;
  time_t last_http_publish;

  TList objects;
  TList gates;
  std::vector<TFile*> cut_files;

  TDirectory* default_directory;
  THttpServer* http_server;
  std::shared_ptr<TLiveHistogramWSHandler> live_ws_handler;
  std::set<TObject*> http_registered_objects;

  TRuntimeObjects obj;

  ClassDef(TCompiledHistograms, 0);
};

#endif /* _TCOMPILEDHISTOGRAMS_H_ */
