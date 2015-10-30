#ifndef _TPIPELINE_H_
#define _TPIPELINE_H_

#include <string>
#include <vector>

#include "StoppableThread.h"
#include "ThreadsafeQueue.h"
#include "TRawEvent.h"
#include "TRawEventSource.h"
#include "TUnpackedEvent.h"

class TPipeline {
public:
  TPipeline();
  virtual ~TPipeline();

  bool CanStart(bool print_reason = false);
  void Initialize();

  void Start();
  void Stop();
  void Join();
  void ProgressBar();

  /// Adds a raw data file.  If called multiple times, will open files sequentially.
  void AddRawDataFile(std::string filename);

  /// Sets the output root file.
  void SetOutputRootFile(std::string filename);

  /// Sets the histogram library to be used.
  void SetHistogramLibrary(std::string filename);

  /// When reading a raw data file, should it be repeatedly checked for new data.
  void SetIsOnline(bool is_online);

  /// Should the input data be time ordered prior to event building.
  void SetTimeOrdering(bool time_order);

  /// Adds an input root file.  If called multiple times, will open the files in a TChain.
  void AddInputRootFile(std::string filename);

  /// Sets the input ring.
  void SetInputRing(std::string ringname);

private:
  void SetupRawReadLoop();
  void SetupRootReadLoop();
  void SetupHistogramLoop();
  void SetupOutputLoop();
  TRawEventSource* OpenSingleFile(const std::string& filename);

  // Output of the TDataLoop.  Unused if reading from a root file.
  ThreadsafeQueue<TRawEvent> raw_event_queue;

  // Output of the TUnpackLoop, or the TRootInputLoop, depending on settings
  ThreadsafeQueue<TUnpackedEvent*> unpacked_event_queue;

  // Output of the THistogramLoop
  ThreadsafeQueue<TUnpackedEvent*> post_histogram_queue;

  std::vector<StoppableThread*> pipeline;

  // The setup parameters
  std::vector<std::string> input_raw_files;
  std::vector<std::string> input_root_files;
  std::string input_ring;

  std::string histogram_library;
  std::string output_root_file;
  bool is_online;

  bool time_order;
  int time_order_depth;
};

#endif /* _TPIPELINE_H_ */
