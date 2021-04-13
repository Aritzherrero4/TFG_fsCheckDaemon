#include <filesystem>
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;
extern "C" int getPathFromConfig(fs::path configFile, fs::path *cp);
