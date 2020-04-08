#pragma once

// std::filesystem has some horrible support in compilers
// so we need this to make sure everything works somewhat

#if __has_include(<filesystem>)
#   include <filesystem>
namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#   include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#elif
#   error "filesystem support is missing"
#endif
