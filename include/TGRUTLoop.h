#ifndef _TGRUTLOOP_H_
#define _TGRUTLOOP_H_

#include "TNamed.h"

#include "TDataLoop.h"
#include "TRootOutfile.h"

// So that rootcint doesn't see this
class RawDataQueue;
class TRootOutfile;
class TNSCLEvent;
class TGEBEvent;

class TGRUTLoop : public TDataLoop {
public:
  static TGRUTLoop* Get();

  virtual ~TGRUTLoop();
  virtual int ProcessEvent(TRawEvent& event);

  using TDataLoop::ProcessFile;
  void ProcessFile(const char* input, const char* output);
  void ProcessFile(const std::vector<std::string>& input, const char* output);

  // Part of the hack to loop over a tree.
  void ProcessTree(TTree* input, const char* output);

  void PrintQueue();
  void StatusQueue();

  virtual bool Initialize();
  virtual void Finalize();

  void PrintOutfile();

  void Status();

  TRootOutfile *GetRootOutfile() { return outfile; }

private:
  template<typename T, typename... Params>
  friend void TDataLoop::CreateDataLoop(Params&&... params);

  void InitOutfile(kFileType file_type, const char* output);

  void ProcessFromQueue(TRawEvent& event);

  TGRUTLoop();
  void WriteLoop();

  void HandleBuiltNSCLData(TNSCLEvent& event);
  void HandleUnbuiltNSCLData(TNSCLEvent& event);
  void HandleGEBData(TGEBEvent& event);
  void HandleGEBMode3(TGEBEvent& event, kDetectorSystems system);

  RawDataQueue* queue;
  TRootOutfile* outfile;

  // Temporary hack to be cleaned up later.
  // If a tree has been given, then WriteLoop() will loop over the tree.
  // Otherwise, it will loop over the raw data.
  TTree* tree;
  void TreeLoop();

#ifndef __CINT__
  std::thread write_thread;
#endif


  ClassDef(TGRUTLoop,0);
};

#endif /* _TGRUTLOOP_H_ */
