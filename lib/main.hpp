#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <thread>

using namespace std;

extern bool debug;
extern bool silent;
extern string outFile;

enum LogLevel {
    DEBUG = 1,
    INFO = 2,
    ERROR = 3,
    IMPORTANT = 4
};