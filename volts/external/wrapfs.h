#pragma once

// std::filesystem has some horrible support in compilers
// so we need this to make sure everything works somewhat

#ifdef NO_FILESYSTEM
#   include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#   include <filesystem>
namespace fs = std::filesystem;
#endif