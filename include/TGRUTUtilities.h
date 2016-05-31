#ifndef __TGRUTUTILITIES_H_
#define __TGRUTUTILITIES_H_

#include <string>
#include <vector>

#include "TGRUTTypes.h"

bool file_exists(const char *filename);
bool all_files_exist(const std::vector<std::string>& filenames);

std::string get_short_filename(std::string filename);
std::string get_run_number(std::string input);

EColor FindColor(std::string);

void CalculateParameters();

#endif
