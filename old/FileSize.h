#ifndef _FILESIZE_H_
#define _FILESIZE_H_

#include <fstream>
#include <ios>

inline size_t FindFileSize(const char* fname) {
  ifstream temp;
  temp.open(fname, std::ios::in | std::ios::ate);
  size_t fsize = temp.tellg();
  temp.close();
  return fsize;
}

#endif /* _FILESIZE_H_ */
