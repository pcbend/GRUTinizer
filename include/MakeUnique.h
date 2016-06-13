#ifndef _MAKEUNIQUE_H_
#define _MAKEUNIQUE_H_

#include <memory>

/// Makes a unique_ptr of the requested type, forwarding all arguments to the constructor.
/**
   This function exists in C++14 as std::make_unique, but is not present in C++11.
 */
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif /* _MAKEUNIQUE_H_ */
