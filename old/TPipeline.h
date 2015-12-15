#ifndef _TPIPELINE_H_
#define _TPIPELINE_H_

#include <string>
#include <vector>

#include "TDirectory.h"

#include "StoppableThread.h"
#include "TGRUTTypes.h"
#include "ThreadsafeQueue.h"
#include "TRawEvent.h"
#include "TRawEventSource.h"
#include "TUnpackedEvent.h"

class TDataLoop;
class TUnpackLoop;
class TRootInputLoop;
class THistogramLoop;
class TWriteLoop;
class TTerminalLoop;

class TPipeline {
public:
  TPipeline();
  virtual ~TPipeline();

  bool CanStart(bool print_reason = false);

  int Initialize();
  void Start();
  void Pause();
  void Resume();
  void Stop();
  void Join();
  std::string Status();
  void Write();


  TDirectory* GetDirectory();
  ///////////////////////////////////////////////////////////////
  /// These are safe to called after the pipeline has started ///
  ///////////////////////////////////////////////////////////////
  void ClearHistograms();
  void ReplaceRawDataFile(std::string filename);

  /// Sets the histogram library to be used.
  void SetHistogramLibrary(std::string filename);
  std::string GetHistogramLibrary() const;

  void SetReplaceVariable(const char* name, double value);
  void RemoveVariable(const char* name);
  TList* GetVariables();

  bool InLearningPhase();

  ///////////////////////////////////////////////////////////////
  /// These should only be called before calling "initialize" ///
  ///////////////////////////////////////////////////////////////
  /// Adds a raw data file.  If called multiple times, will open files sequentially.
  void AddRawDataFile(std::string filename);

  /// Sets the output root file.
  void SetOutputRootFile(std::string filename);

  /// When reading a raw data file, should it be repeatedly checked for new data.
  void SetIsOnline(bool is_online);

  /// Should the input data be time ordered prior to event building.
  void SetTimeOrdering(bool time_order);

  /// Adds an input root file.  If called multiple times, will open the files in a TChain.
  void AddInputRootFile(std::string filename);

  /// Sets the input ring.
  void SetInputRing(std::string ringname);

  /// Sets the expected file type
  void SetFiletype(kFileType file_type);

  bool IsFinished();

private:
  TPipeline(const TPipeline&) { }
  TPipeline& operator=(const TPipeline&) { }

  bool AllQueuesEmpty();


  void SetupRawReadLoop();
  void SetupRootReadLoop();
  void SetupOutputFile();
  void SetupHistogramLoop();
  void SetupOutputLoop();
  TRawEventSource* OpenSingleFile(const std::string& filename, bool is_ring=false);

  bool is_initialized;

  TFile* output_file;
  TDirectory* output_directory;

  // Output of the TDataLoop.  Unused if reading from a root file.
  ThreadsafeQueue<TRawEvent> raw_event_queue;

  // Output of the TUnpackLoop, or the TRootInputLoop, depending on settings
  ThreadsafeQueue<TUnpackedEvent*> unpacked_event_queue;

  // Output of the THistogramLoop
  ThreadsafeQueue<TUnpackedEvent*> post_histogram_queue;

  std::vector<StoppableThread*> pipeline;
  TDataLoop* data_loop;
  TUnpackLoop* unpack_loop;
  TRootInputLoop* root_input_loop;
  THistogramLoop* histogram_loop;
  TWriteLoop* write_loop;
  TTerminalLoop* terminal_loop;

  // The setup parameters
  std::vector<std::string> input_raw_files;
  std::vector<std::string> input_root_files;
  std::string input_ring;
  kFileType fFileType;

  std::string histogram_library;
  std::string output_root_file;
  bool is_online;

  bool time_order;
  int time_order_depth;

  ClassDef(TPipeline, 0);
};

#endif /* _TPIPELINE_H_ */
