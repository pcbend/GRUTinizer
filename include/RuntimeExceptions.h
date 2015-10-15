#ifndef _RUNTIMEEXCEPTIONS_H_
#define _RUNTIMEEXCEPTIONS_H_

#include <stdexcept>

struct RuntimeException : public std::runtime_error {
  RuntimeException(const std::string& msg) : std::runtime_error(msg) { }
};

struct RuntimeFileNotFound : public RuntimeException {
  RuntimeFileNotFound(const std::string& msg) : RuntimeException(msg) { }
};

struct RuntimeSymlinkCreation : public RuntimeException {
  RuntimeSymlinkCreation(const std::string& msg) : RuntimeException(msg) { }
};

#endif /* _RUNTIMEEXCEPTIONS_H_ */
