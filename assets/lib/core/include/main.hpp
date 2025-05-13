#pragma once
#include <utility>
#include <vector>
#include <string>
#include "ui.hpp"

typedef std::vector<std::pair<std::string, std::string>> args_t;

enum LogLevel{
    INFO = 0,
    DEBUG = 1,
    ERROR = 2
};