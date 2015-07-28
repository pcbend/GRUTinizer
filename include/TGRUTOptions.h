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
  std::string InputRing() { return input_ring; }

  std::string DetectorEnvironment() { return detector_environment; }

  const std::vector<std::string>& OptionFiles() { return options_file; }

  bool ExitAfterSorting() const { return fExitAfterSorting; }
  bool ShowedHelp() const { return fHelp; }
  bool ShowLogo() const { return fShowLogo; }
  bool SortRaw() const { return fSortRaw; }

  bool ShouldExitImmediately() const { return fShouldExit; }

  kFileType DetermineFileType(const std::string& filename);

private:
  TGRUTOptions(int argc, char** argv);

  bool FileAutoDetect(const std::string& filename);

  std::vector<std::string> input_raw_files;
  std::vector<std::string> input_root_files;
  std::vector<std::string> input_cal_files;
  std::vector<std::string> input_macro_files;
  std::string input_ring;

  std::string detector_environment;

  std::vector<std::string> options_file;

  bool fExitAfterSorting;
  bool fHelp;
  bool fShowLogo;
  bool fSortRaw;

  bool fShouldExit;

  ClassDef(TGRUTOptions,0);
};

#endif /* _TGRUTOPTIONS_H_ */
