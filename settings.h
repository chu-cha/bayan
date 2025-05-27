#pragma once

#include <vector>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

struct Settings {
    std::vector<fs::path> included_dirs;
    std::vector<fs::path> excluded_dirs;
    bool deep_search = false;
    uintmax_t min_size = 1;
    std::vector<std::string> allowed_patterns;
    uintmax_t block_size = 1024;

    enum class hashing_algorithms {
        crc32,
        md5,
        sha1
    };

    hashing_algorithms algorithm = hashing_algorithms::crc32;
};