#ifndef _TGRUTOPTIONS_H_
#define _TGRUTOPTIONS_H_

#include <map>

#include "TObject.h"

class TGRUTOptions : public TObject {
public:
  static TGRUTOptions* Get(int argc = 0, char** argv = NULL);

  void Clear(Option_t* opt = "");
  void Load(int argc, char** argv);
  void Print(Option_t* opt = "") const;

  bool ShouldExit() { return fShouldExit; }

private:
  TGRUTOptions(int argc, char** argv);

  bool FileAutoDetect(const std::string& filename);

  std::vector<std::string> input_raw_files;
  std::vector<std::string> input_root_files;
  std::vector<std::string> input_cal_files;
  std::vector<std::string> input_macro_files;
  std::string input_ring;

  std::vector<std::string> options_file;

  bool fExitAfterSorting;
  bool fHelp;
  bool fNoLogo;
  bool fNoSort;

  bool fShouldExit;

  ClassDef(TGRUTOptions,0);
};

#endif /* _TGRUTOPTIONS_H_ */
