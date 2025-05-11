#pragma once
#include <unordered_map>
#include <string>
#include <vector>

void printConsole(std::string par, int priotity);
std::unordered_map<std::string, std::string> parseArgs(int argc, char* argv[]);
void inputThread(std::string* input, bool* hasInput, bool* run);
std::string inputR(std::string tag);
std::vector<std::string> splitString(std::string s, std::string delimiter);