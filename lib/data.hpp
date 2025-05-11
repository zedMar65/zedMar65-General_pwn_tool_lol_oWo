#pragma once
#include <string>

struct pythonProcessData;
class SeshionData;

std::string* get_data(pythonProcessData* process);
std::string* get_err(pythonProcessData* process);
int get_id(pythonProcessData* process);
void stop_proces(pythonProcessData* process);
void asign_id(pythonProcessData *process, int _id);