#pragma once
#include "main.hpp"
#include "sessionData_internal.hpp"
#include <memory>


struct pythonProcessData;
class SessionData;

struct Session{
    std::unique_ptr<SessionData> seshData;
    Session();
    std::string session_status();
    void set_arg(std::string _key, std::string _value);
    std::string get_arg(std::string _key);
    void erase_arg(std::string _key);
    pythonProcessData* get_process(int _id);
    pythonProcessData* get_process(std::string _id_str);
    int new_process(std::string _name, args_t _args, std::string _code_type, bool _await);
    void run_process(int _id, bool _await);
    void run_process(string _id_str, bool _await);
    void stop_process(int _id);
    void stop_process(std::string _id_str);
    void erase_process(int _id);
    void erase_process(std::string _id_str);
    void clear_processes();
    void quit_session();
    std::string find_arg(std::string _key);
    std::string get_raw_arg(std::string _key);
};

