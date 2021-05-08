#ifndef UTILS_HPP
#define UTILS_HPP

#include <filesystem>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string.h>

namespace fs = std::filesystem;
extern "C" int getPathFromConfig(fs::path configFile, fs::path *cp);
extern "C" int getHashModeFromConfig(fs::path configFile, int *mode);

#endif