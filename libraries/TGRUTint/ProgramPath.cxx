#include "ProgramPath.h"

#ifdef __linux__

#include <unistd.h>
#include <limits.h>

std::string program_path(){
  char buff[PATH_MAX+1];
  size_t len = readlink("/proc/self/exe", buff, sizeof(buff)-1);
  buff[len] = '\0';

  std::string exe_path = buff;
  return exe_path.substr(0, exe_path.find_last_of('/'));
}
#endif


#ifdef _WIN32

#include <windows.h>

// Code from http://stackoverflow.com/questions/2647429
std::string program_path(){
  HMODULE hModule = GetModuleHandleW(NULL);
  wchar_t path[MAX_PATH];
  GetModuleFileNameW(hModule, path, MAX_PATH);

  std::wstring ws(path);
  std::string exe_path(ws.begin(), ws.end());
  return exe_path.substr(0, exe_path.find_last_of('\\'));
}

#endif
