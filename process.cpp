#include <memory>
#include <cstring>
#include <algorithm>

#include "process.h"
#include "block_hash.h"

std::string FileReader::readNextBlock(unsigned long long block_size) const {
    if (!stream || !stream->is_open()) return "";

    std::vector<char> buffer(block_size);
    stream->read(buffer.data(), block_size);
    size_t bytesRead = stream->gcount();

    if (bytesRead == 0) return "";

    if (bytesRead < block_size) {
        // fill remained  part with zeroes
        memset(buffer.data() + bytesRead, 0, block_size - bytesRead);
    }

    return computeBlockHash(buffer.data(), bytesRead, hash_algorithm);
}

bool CompareFiles::matchesPattern(const std::filesystem::path& filepath,
    const std::vector<std::string>& patterns) const {
    if (patterns.empty()) return true; // No masks - allow all

    std::string filename = filepath.filename().string();
    std::string ext = filepath.extension().string();

    // make all lowercase
    std::transform(filename.begin(), filename.end(), filename.begin(),
        [](unsigned char c) { return std::tolower(c); });

    for (const auto& pattern : patterns) {
        std::string low_pattern = pattern;
        std::transform(low_pattern.begin(), low_pattern.end(), low_pattern.begin(),
            [](unsigned char c) { return std::tolower(c); });

        // simple check by extension
        if (low_pattern.find("*.") == 0) {
            std::string pattern_ext = low_pattern.substr(1);
            if (ext.size() >= pattern_ext.size() &&
                std::equal(pattern_ext.rbegin(), pattern_ext.rend(), ext.rbegin(),
                    [](char a, char b) { return a == '?' || a == b; })) {
                return true;
            }
        }

        // check full names
        if (pattern == "*" || pattern == "*.*") return true;
    }

    return false;
}

void CompareFiles::collectFiles()
{
    for (const auto& d : settings.included_dirs)
        collectFiles(
            d,
            settings.excluded_dirs,
            settings.deep_search,
            settings.min_size,
            settings.allowed_patterns);
}

void CompareFiles::collectFiles(const fs::path& directory,
    const std::vector<fs::path>& excluded_dirs,
    bool recursive,
    uintmax_t min_file_size,
    const std::vector<std::string>& allowed_patterns)
{
    try {
        for (const auto& entry : fs::directory_iterator(directory)) {
            // dismiss excluded directories
            if (entry.is_directory()) {

                if (std::find_if(excluded_dirs.begin(), excluded_dirs.end(),
                    [&entry](const fs::path& excluded) {
                        return excluded == entry.path();
                    }) != excluded_dirs.end()) {
                    continue;
                }

                if (recursive)
                    // recursive pass
                    collectFiles(entry.path(), excluded_dirs, recursive, min_file_size, allowed_patterns);
            }
            else if (entry.is_regular_file()) {
                try {
                    const auto file_size = entry.file_size();
                    if (file_size >= min_file_size && matchesPattern(entry.path(), allowed_patterns)) {
                        file_readers.push_back(FileReader(entry.path(), settings.algorithm));
                    }
                }
                catch (const fs::filesystem_error& size_err) {
                    std::cerr << "Error getting size for " << entry.path()
                        << ": " << size_err.what() << "\n";
                }
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error accessing " << directory << ": " << e.what() << "\n";
    }
}

void CompareFiles::groupFilesByBlocks() {
    collectFiles();

    TrieNode root;

    TrieNode* currentLevel = &root;
    bool allFilesProcessed;

    std::vector<TrieNode*> currentLevelHashes(file_readers.size(), currentLevel);

    do {
        allFilesProcessed = true;

        for (int reader_id = 0; reader_id < file_readers.size(); reader_id++) {

            auto& reader = file_readers[reader_id];
            currentLevel = currentLevelHashes[reader_id];

            if (!reader.is_valid()) {  // means this file has been already processed
                continue;

            }

            std::string blockHash = reader.readNextBlock(settings.block_size);

            if (blockHash.empty()) {
                reader.reset(); // file ended
                currentLevel->filenames.push_back(reader.getFilePath());
                continue;
            }

            allFilesProcessed = false;

            if (currentLevel->children.find(blockHash) == currentLevel->children.end()) {
                currentLevel->children[blockHash] = std::make_unique<TrieNode>();
                currentLevel->children[blockHash]->blockHash = blockHash;
            }

            currentLevel = currentLevel->children[blockHash].get();
            currentLevelHashes[reader_id] = currentLevel;

        }

    } while (!allFilesProcessed);

    auto fileGroups = collectGroups(&root);

    for (const auto& group : fileGroups) {
        for (const auto& file : group) {
            std::cout << file << "\n";
        }
        std::cout << "\n";
    }
}

std::vector<std::vector<fs::path>> CompareFiles::collectGroups(TrieNode* node) {
    std::vector<std::vector<fs::path>> fileGroups;

    if (!node->filenames.empty()) {
        fileGroups.push_back(node->filenames);
    }
    for (const auto& [hash, child] : node->children) {
        auto fgroups = collectGroups(child.get());
        fileGroups.insert(fileGroups.end(), fgroups.begin(), fgroups.end());
    }

    return fileGroups;
};