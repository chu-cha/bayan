#include <memory>
#include <openssl/sha.h>
#include <sstream>
#include <cstring>
#include <algorithm>

#include "process.h"

std::string computeBlockHash(const char* data, size_t size) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data), size, hash);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

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

    return computeBlockHash(buffer.data(), bytesRead);  //TODO
    //return std::string(buffer.begin(), buffer.end());  //TODO
}

void CompareFiles::collectFiles(){
    for (const auto& d: settings.included_dirs)
        collectFiles(d, settings.excluded_dirs, settings.deep_search, settings.min_size);
}


void CompareFiles::collectFiles(const fs::path& directory,
    const std::vector<fs::path>& excluded_dirs,
    bool recursive,
    uintmax_t min_file_size)
{
    try {
        for (const auto& entry : fs::directory_iterator(directory)) {
            // dismiss excluded directories
            if (entry.is_directory()) {

                if (std::find_if(excluded_dirs.begin(), excluded_dirs.end(),
                    [&entry](const fs::path& excluded) {
                        return excluded == entry.path();
                    }) != excluded_dirs.end()) {
                   // std::cout << "[SKIPPED] " << entry.path() << "\n";
                    continue;
                }

                if (recursive)
                    // recursive pass
                    collectFiles(entry.path(), excluded_dirs, recursive, min_file_size);
            }
            else if (entry.is_regular_file()) {
                //std::cout << "[FILE] " << entry.path() << "\n";
                try {
                    const auto file_size = entry.file_size();
                    if (file_size >= min_file_size) {
                        file_readers.push_back(entry.path());
                    }
                    // else файл слишком мал, пропускаем
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
            std::cout << reader.getFilePath() << ": " << blockHash.empty() << " " << blockHash << std::endl;
             
            if (blockHash.empty()) {
                reader.reset(); // file ended
                currentLevel->filenames.push_back(reader.getFilePath());
                std::cout << "File ended and writed to " << currentLevel->blockHash<<"\n";
                continue;
            }

            allFilesProcessed = false;

            if (currentLevel->children.find(blockHash) == currentLevel->children.end()) {
                currentLevel->children[blockHash] = std::make_unique<TrieNode>();
                currentLevel->children[blockHash]->blockHash = blockHash;
            }

            currentLevel = currentLevel->children[blockHash].get();
            currentLevelHashes[reader_id] = currentLevel;
            std::cout << reader_id << ": " << currentLevelHashes[reader_id]->blockHash<< std::endl;

        }

    } while (!allFilesProcessed);

    auto fileGroups = collectGroups(&root);

    for (const auto& group : fileGroups) {
            std::cout << "Grouped files:\n";
            for (const auto& file : group) {
                std::cout << "  " << file << "\n";
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