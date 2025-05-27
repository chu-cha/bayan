#include "parse_command_line.h"

#include <iostream>

namespace boost::program_options {
    template<>
    void validate(boost::any& v,
        const std::vector<std::string>& values,
        Settings::hashing_algorithms*,
        long)
    {
        static const std::map<std::string, Settings::hashing_algorithms> mapping = {
            {"crc32", Settings::hashing_algorithms::crc32},
            {"md5",   Settings::hashing_algorithms::md5},
            {"sha1",  Settings::hashing_algorithms::sha1}
        };

        validators::check_first_occurrence(v);
        const std::string& s = validators::get_single_string(values);

        auto it = mapping.find(s);
        if (it == mapping.end()) {
            throw validation_error(validation_error::invalid_option_value);
        }

        v = it->second;
    }

    template<>
    void validate(boost::any& v,
        const std::vector<std::string>& values,
        fs::path*,
        long)
    {
        po::validators::check_first_occurrence(v);
        v = fs::path(po::validators::get_single_string(values));
    }
}

int parse_command_line(int argc, char* argv[], Settings& settings) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("dir,d", po::value<std::vector<fs::path>>(&settings.included_dirs)->multitoken(), "search in directories")
        ("exdir,x", po::value<std::vector<fs::path>>(&settings.excluded_dirs)->multitoken(), "exclude directories")
        ("level,l", po::value<bool>(&settings.deep_search), "use deep scanning")
        ("min_size,s", po::value<uintmax_t>(&settings.min_size), "set min file size")
        ("block_size,b", po::value<uintmax_t>(&settings.block_size), "set block size ")
        ("hash_algo,a", po::value<typename Settings::hashing_algorithms>(&settings.algorithm), "set hashing algorithm")
        ;

    /*
         • маски имен файлов разрешенных для сравнения(не зависят от
             регистра)
    */

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    }
    catch (const po::error& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }


    if (vm.count("dir")) {

        std::cout << "Search directory is: \n";

        for (const auto& d : settings.included_dirs)
            std::cout << '\t' << d << "\n";
    }

    if (vm.count("exdir")) {

        std::cout << "Excluded directory is: \n";
        for (const auto& d : settings.excluded_dirs)
            std::cout << '\t' << d << "\n";
    }

    if (vm.count("level")) {
        std::cout << "Scanning level was set to: "
            << settings.deep_search << "\n";
    }

    if (vm.count("min_size")) {
        std::cout << "Min file size: "
            << settings.min_size << "\n";
    }

    if (vm.count("block_size")) {
        std::cout << "Block size: "
            << settings.block_size << "\n";
    }

    std::cout << "Selected algorithm: ";
    switch (settings.algorithm) {
    case Settings::hashing_algorithms::crc32: std::cout << "CRC32\n"; break;
    case Settings::hashing_algorithms::md5:   std::cout << "MD5\n";   break;
    case Settings::hashing_algorithms::sha1:  std::cout << "SHA1\n";  break;
    }

    return 0;
}