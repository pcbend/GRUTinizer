#include "TGlobRawFile.h"

#include <glob.h>

namespace {
  // From http://stackoverflow.com/a/8615450/2689797
  std::vector<std::string> glob(const std::string& pattern) {
    glob_t glob_result;
    glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);

    std::vector<std::string> output;
    for(unsigned int i=0; i<glob_result.gl_pathc; i++) {
      output.push_back(glob_result.gl_pathv[i]);
    }
    globfree(&glob_result);

    return output;
  }

  const std::chrono::system_clock::duration time_between_checks =
    std::chrono::seconds(10);
}

TGlobRawFile::TGlobRawFile(std::string pattern)
  : fPattern(pattern),
    fPreviousCheck(std::chrono::system_clock::now() - 2*time_between_checks) { }

void TGlobRawFile::CheckForFiles() {
  auto now = std::chrono::system_clock::now();
  auto time_since_check = fPreviousCheck - now;
  if(time_since_check > time_between_checks) {
    return;
  }

  for(auto& filename : glob(fPattern)) {
    if(fFilesAdded.count(filename) == 0) {
      auto new_file = TRawEventSource::EventSource(filename.c_str());
      fWrapped.AddFile(new_file);
      fFilesAdded.insert(filename);
    }
  }


  fPreviousCheck = now;
}
