#include "sessionData.hpp"
#include "main.hpp"
#include "pythonProcess.hpp"
#include "pythonProcess_internal.hpp"
#include "sessionData_internal.hpp"
#include "ui.hpp"
#include <memory>
#include <string>
using namespace std;

Session::Session(){
    seshData = make_unique<SessionData>();
    seshData->setArg("run", "true");
}

string Session::session_status(){
    string data = "\"variables\": [\n";
    vector<string> keys = seshData->getAllKeys();
    for(int i = 0; i < keys.size(); i++){
        data +="{\"key\": \"" + keys[i] + "\"" + ", \"value\": \"" + get_raw_arg(keys[i]) + "\"}";
        if (i != keys.size()-1){
            data += ",\n";
        } 
    }
    data += "],\n\"processes\": [\n";
    int proc_len = seshData->getProcessLen();
    for(int i = 0; i < proc_len; i++){
        data +="{\"id\": \"" + to_string(i) + "\"" + ", \"status\": \"" + to_string(is_alive(*seshData->getProcess(i))) + "\", \"name\": \""+ get_name(*seshData->getProcess(i)) +"\"}";
        if (i != proc_len-1){
            data += ",\n";
        }  
    }
    data += "]\n}";
    return data;
}

void Session::set_arg(string _key, string _value){
    _key = find_arg(_key);
    seshData->setArg(_key, _value);
    return;
}

string Session::get_arg(string _key){
    _key = find_arg(_key);
    if (!seshData->evalKey(_key)){
        log("Key not recognized", ERROR);
        return "";
    }
    return find_arg(seshData->getArg(_key));
}

void Session::erase_arg(string _key){
    _key = find_arg(_key);
    if (!seshData->evalKey(_key)){
        log("Key not recognized", ERROR);
        return;
    }
    seshData->eraseArg(_key);
    return;
}

pythonProcessData* Session::get_process(int _id){
    if (_id < 0 || _id >= seshData->getProcessLen()){
        log("Key not recognized", ERROR);
        return nullptr;
    }
    return seshData->getProcess(_id);
}

pythonProcessData* Session::get_process(string _id_str){
    int _id = stoi(find_arg(_id_str));
    if (_id < 0 || _id >= seshData->getProcessLen()){
        log("Key not recognized", ERROR);
        return nullptr;
    }
    return seshData->getProcess(_id);
}

int Session::new_process(string _name, args_t _args, string _code_type, bool _await = false){
    int _id = seshData->startProcess(find_arg(_name), _args);
    pythonProcessData& process = *get_process(_id);
    code_info code = get_code(_code_type);
    for(int i = 0; i < code.first.size(); i++){
        code.second = code.first[i].first + "=" + find_arg(get_const(code.first[i].second)) + "\n" + code.second;
    }
    code.second = get_code("STD_REDIRECT").second + code.second;
    run_code(process, _await);
    return _id;
}

void Session::run_process(int _id, bool _await = false){
    run_code(*get_process(_id), _await);
}

void Session::run_process(string _id_str, bool _await = false){
    run_code(*get_process(_id_str), _await);
}

void Session::stop_process(int _id){
    if (_id < 0 || _id >= seshData->getProcessLen()){
        log("Key not recognized", ERROR);
        return;
    }
    stop_process_i(*get_process(_id));
    return;
}

void Session::stop_process(string _id_str){
    int _id = stoi(find_arg(_id_str));
    if (_id < 0 || _id >= seshData->getProcessLen()){
        log("Key not recognized", ERROR);
        return;
    }
    stop_process_i(*get_process(_id));
    return;
}

void Session::erase_process(int _id){
    if (_id < 0 || _id >= seshData->getProcessLen()){
        log("Key not recognized", ERROR);
        return;
    }
    stop_process(_id);
    seshData->eraseProcess(_id);
}

void Session::erase_process(string _id_str){
    int _id = stoi(find_arg(_id_str));
    if (_id < 0 || _id >= seshData->getProcessLen()){
        log("Key not recognized", ERROR);
        return;
    }
    stop_process(_id);
    seshData->eraseProcess(_id);
}

void Session::clear_processes(){
    int len = seshData->getProcessLen();
    for(int i = 0; i < len; i++){
        if(!is_alive(*seshData->getProcess(i))){
            seshData->eraseProcess(i);
        }
    }
    return;
}

void Session::quit_session(){
    int len = seshData->getProcessLen();
    for(int i = 0; i < len; i++){
        erase_process(i);
    }
    return;
}

string Session::find_arg(string _key){
    if(_key[0]!='$'){
        return _key;
    }
    if (!seshData->evalKey(_key.substr(1))){
        log("Key not recognized", ERROR);
        return _key;
    }
    if (get_raw_arg(_key.substr(1)) == _key){
        log("Key points to itself", ERROR);
        return _key;
    }
    return find_arg(get_arg(_key.substr(1)));
}

string Session::get_raw_arg(string _key){
    return seshData->getArg(_key);
}