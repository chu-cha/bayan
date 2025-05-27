#pragma once

#include "settings.h"

std::string computeBlockHash(const char* data, size_t size, Settings::hashing_algorithms algo);
