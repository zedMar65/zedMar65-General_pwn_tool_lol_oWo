#pragma once
#include <utility>
#include <vector>
#include <string>


typedef std::vector<std::pair<std::string, std::string>> args_t;
typedef std::pair<args_t, std::string> code_info;

enum LogLevel{
    INFO = 0,
    DEBUG = 1,
    ERROR = 2
};

std::string get_const(std::string _key);
std::pair<args_t, std::string> get_code(std::string _key);

// depricated functions, avoid usage
void _mod_const(std::string _key, std::string _value);
void _mod_code(std::string _key, std::pair<args_t, std::string> _value);