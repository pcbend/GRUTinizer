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
  const std::vector<std::string>& RootInputFiles()  { return input_root_files;  }
  const std::vector<std::string>& CalInputFiles()   { return input_cal_files;   }
  const std::vector<std::string>& MacroInputFiles() { return input_macro_files; }
  const std::string DetectorEnvironment()           { return detector_environment; }
  const std::string OutputFile()                    { return output_file; }
  std::string InputRing() { return input_ring; }

  int Port() { return fPort; }

  const std::vector<std::string>& OptionFiles() { return options_file; }

  bool IgnoreErrors() { return fIgnoreErrors; }

  bool ExitAfterSorting() const { return fExitAfterSorting; }
  bool ShowedHelp() const { return fHelp; }
  bool ShowLogo() const { return fShowLogo; }
  bool SortRaw() const { return fSortRaw; }
  bool IgnoreMode3() const { return fIgnoreMode3; }

  bool ShouldExitImmediately() const { return fShouldExit; }

  kFileType DetermineFileType(const std::string& filename);
  std::string GenerateOutputFilename(const std::string& filename);
  std::string GenerateOutputFilename(const std::vector<std::string>& filename);

private:
  TGRUTOptions(int argc, char** argv);

  bool FileAutoDetect(const std::string& filename);

  std::vector<std::string> input_raw_files;
  std::vector<std::string> input_root_files;
  std::vector<std::string> input_cal_files;
  std::vector<std::string> input_macro_files;
  std::string input_ring;

  std::string output_file;

  std::string detector_environment;

  std::vector<std::string> options_file;

  int fPort;

  bool fExitAfterSorting;
  bool fHelp;
  bool fShowLogo;
  bool fSortRaw;
  bool fIgnoreErrors;
  bool fIgnoreMode3;

  bool fShouldExit;

  ClassDef(TGRUTOptions,0);
};

#endif /* _TGRUTOPTIONS_H_ */
