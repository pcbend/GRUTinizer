#ifndef __TGRUTUTILITIES_H_
#define __TGRUTUTILITIES_H_

#include <string>

#include "TGRUTTypes.h"

bool file_exists(const char *filename);

std::string get_short_filename(std::string filename);
std::string get_run_number(std::string input);

EColor FindColor(std::string);

void CalculateParameters();

#endif
