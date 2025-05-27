#pragma once
#include <string>
#include <unordered_map>
#include <fstream>

#include "settings.h"


class FileReader {
public:
    FileReader(const fs::path& path, Settings::hashing_algorithms algorithm)
        : filePath(path),
        hash_algorithm(algorithm),
        stream(std::make_unique<std::ifstream>(path, std::ios::binary)) {
    }

    ~FileReader()= default;
    FileReader(const FileReader&) = delete;
    FileReader& operator=(const FileReader&) = delete;
    FileReader(FileReader&&) = default;
    FileReader& operator=(FileReader&&) = default;

    fs::path getFilePath() const { return filePath; }

    bool is_valid() const { return static_cast<bool>(stream);  }
    void reset() { stream.reset(); }

    std::string readNextBlock(unsigned long long block_size) const;

private:
    fs::path filePath;
    std::unique_ptr<std::ifstream> stream;
    Settings::hashing_algorithms hash_algorithm;

};

class CompareFiles {
public:
    CompareFiles(const Settings& sett) : settings(sett) {}

    void groupFilesByBlocks() ;

private:
    class TrieNode {
    public:
        std::string blockHash;
        std::vector < fs::path > filenames; // files, that are ended and the same at this particular leaf
        std::unordered_map< std::string, std::unique_ptr<TrieNode>> children;
    };

    std::vector<FileReader> file_readers;
    Settings settings;

private:

    void collectFiles();

    void collectFiles(const fs::path& directory,
        const std::vector<fs::path>& excluded_dirs,
        bool recursive = false,
        uintmax_t min_file_size = 1,
        const std::vector<std::string>& patterns = {});

    bool matchesPattern(const std::filesystem::path& filepath, const std::vector<std::string>& patterns) const;

    std::vector<std::vector<fs::path>> collectGroups(TrieNode* node);

};
