#include <openssl/sha.h>
#include <openssl/md5.h>
#include <boost/crc.hpp>
#include <sstream>

#include "block_hash.h"

std::string computeBlockHash(const char* data, size_t size, Settings::hashing_algorithms algo) {

    using hashing_algorithms = Settings::hashing_algorithms;

    std::stringstream ss;

    switch (algo) {
    case hashing_algorithms::crc32: {
        boost::crc_32_type result;
        result.process_bytes(data, size);
        ss << std::setw(8) << result.checksum();
        break;
    }
    case hashing_algorithms::md5: {
        unsigned char hash[MD5_DIGEST_LENGTH];
        MD5(reinterpret_cast<const unsigned char*>(data), size, hash);
        for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
        }
        break;
    }
    case hashing_algorithms::sha1: {
        unsigned char hash[SHA_DIGEST_LENGTH];
        SHA1(reinterpret_cast<const unsigned char*>(data), size, hash);
        for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
        }
        break;
    }

    default:
        throw std::runtime_error("Unsupported hashing algorithm");
    }

    return ss.str();
}
