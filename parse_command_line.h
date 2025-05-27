#pragma once
#include "settings.h"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace boost::program_options {
    template<>
    void validate(boost::any& v,
        const std::vector<std::string>& values,
        Settings::hashing_algorithms*,
        long);
    

    template<>
    void validate(boost::any& v,
        const std::vector<std::string>& values,
        fs::path*,
        long);
}

int parse_command_line(int argc, char* argv[], Settings& settings);