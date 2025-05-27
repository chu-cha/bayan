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
        ("patterns,p", po::value<std::vector<std::string>>(&settings.allowed_patterns)->multitoken(),"File patterns to compare (*.txt, *.jpg etc)");
        ;

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

    return 0;
}