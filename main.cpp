#include <iostream>
#include <string>
#include <vector>

#include <filesystem>

#include "parse_command_line.h"
#include "process.h"





int main(int argc, char* argv[]) {

    Settings settings;
    bool parse_res = parse_command_line(argc, argv, settings);
    if (parse_res != 0)
        return parse_res;

    CompareFiles comparator(settings);
    comparator.groupFilesByBlocks();

    return 0;
}