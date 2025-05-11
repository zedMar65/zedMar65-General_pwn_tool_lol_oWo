#pragma once
#include "main.hpp"
#include "pythonProcess.hpp"
#include <memory>

class SessionData;

std::unique_ptr<SessionData> start_session();
std::string session_status(SessionData &seshData);
void set_arg(SessionData &seshData, std::string _key, std::string _value);
std::string get_arg(SessionData &seshData, std::string _key);
pythonProcessData& get_process(SessionData &seshData, int _id);
int new_process(SessionData &seshData, std::string _name, args_t args);
void erase_process(SessionData &seshData, int _id);
void clear_processes(SessionData &seshData);
void quit_session(SessionData &seshData);