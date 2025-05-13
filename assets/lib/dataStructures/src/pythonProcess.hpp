#pragma once
#include <string>
#include <memory>
#include "main.hpp"
#include "pythonProcess_internal.hpp"

std::string* get_err(pythonProcessData &process);
std::string* get_data(pythonProcessData &process);
int* get_id(pythonProcessData &process);
void stop_process_i(pythonProcessData &process);
void asign_id(pythonProcessData &process, int _id);
bool is_alive(pythonProcessData &process);
std::string get_name(pythonProcessData &process);
std::unique_ptr<pythonProcessData> start_process(std::string _name, args_t _args, int _id);