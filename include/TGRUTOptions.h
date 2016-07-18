#ifndef _TGRUTOPTIONS_H_
#define _TGRUTOPTIONS_H_

#include <map>

#include "TObject.h"

#include "TGRUTTypes.h"

class TGRUTOptions : public TObject {
public:
  static TGRUTOptions* Get(int argc = 0, char** argv = NULL);

  void Clear(Option_t* opt = "");
  void Load(int argc, char** argv);
  void Print(Option_t* opt = "") const;

  bool ShouldExit() { return fShouldExit; }
  const std::vector<std::string>& RawInputFiles()   { return input_raw_files;   }
  const std::vector<std::string>& GetRawInputFiles(){ return input_raw_files;   }
  const std::vector<std::string>& RootInputFiles()  { return input_root_files;  }
  const std::vector<std::string>& CalInputFiles()   { return input_cal_files;   }
  const std::vector<std::string>& ValInputFiles()   { return input_val_files;   }
  const std::vector<std::string>& CutsInputFiles()  { return input_cuts_files;   }
  const std::vector<std::string>& WinInputFiles()   { return input_win_files;   }
  const std::vector<std::string>& MacroInputFiles() { return input_macro_files; }
  const std::vector<std::string>& GuiSaveSetFiles() { return input_gui_hist_files; }
  const std::string& DetectorEnvironment()          { return detector_environment; }
  const std::string& OutputFile()                   { return output_file; }
  const std::string& OutputFilteredFile()        { return output_filtered_file; }
  const std::string& OutputHistogramFile()          { return output_histogram_file; }
  const std::string& S800InverseMapFile()           { return s800_inverse_map_file; }
  std::string InputRing() { return input_ring; }
  std::string CompiledHistogramFile() { return compiled_histogram_file; }
  std::string CompiledFilterFile() { return compiled_filter_file; }

  const std::vector<std::string>& OptionFiles() { return options_file; }

  int BuildWindow() const { return fBuildWindow; }

  bool ExitAfterSorting()   const { return fExitAfterSorting; }
  bool ShowedHelp()         const { return fHelp; }
  bool ShowedVersion()      const { return fShowedVersion; }
  bool ShowLogo()           const { return fShowLogo; }
  bool SortRaw()            const { return fSortRaw; }
  bool SortRoot()           const { return fSortRoot; }
  bool ExtractWaves()       const { return fExtractWaves;  }
  bool StartGUI()           const { return fStartGui; }
  bool MakeHistos()         const { return fMakeHistos; }
  bool SortMultiple()       const { return fSortMultiple; }


  bool IsOnline()           const { return fIsOnline; }

  bool TimeSortInput()      const { return fTimeSortInput; }
  int TimeSortDepth()       const { return fTimeSortDepth; }

  bool ShouldExitImmediately() const { return fShouldExit; }

  kFileType   DefaultFileType() const { return fDefaultFileType; }
  kFileType   DetermineFileType(const std::string& filename) const;

  std::string GenerateOutputFilename(const std::string& filename);
  std::string GenerateOutputFilename(const std::vector<std::string>& filename);

  bool LongFileDescription() const { return fLongFileDescription; }


  void SetStartGUI(bool flag=true) { fStartGui=flag; }

private:
  TGRUTOptions(int argc, char** argv);

  bool FileAutoDetect(const std::string& filename);

  std::vector<std::string> input_raw_files;
  std::vector<std::string> input_root_files;
  std::vector<std::string> input_cuts_files;
  std::vector<std::string> input_cal_files;
  std::vector<std::string> input_val_files;
  std::vector<std::string> input_win_files;
  std::vector<std::string> input_macro_files;
  std::vector<std::string> input_gui_hist_files;
  std::string input_ring;

  std::string output_file;
  std::string output_filtered_file;
  std::string output_histogram_file;

  std::string detector_environment;
  std::string compiled_histogram_file;
  std::string compiled_filter_file;
  std::string s800_inverse_map_file;

  std::vector<std::string> options_file;

  kFileType fDefaultFileType;
  bool fExitAfterSorting;
  bool fShowedVersion;
  bool fHelp;
  bool fShowLogo;
  bool fSortRaw;
  bool fSortRoot;
  bool fExtractWaves;
  bool fIsOnline;
  bool fStartGui;
  bool fMakeHistos;
  bool fSortMultiple;

  bool fTimeSortInput;
  int fTimeSortDepth;

  int fBuildWindow;

  bool fShouldExit;

  bool fLongFileDescription;

  ClassDef(TGRUTOptions,0);
};

#endif /* _TGRUTOPTIONS_H_ */
