#include "pythonProcess_internal.hpp"
#include "pythonProcess.hpp"

using namespace std;

string* get_err(pythonProcessData &process){
    return &process.error;
}

string* get_data(pythonProcessData &process){
    return &process.output;
}

int* get_id(pythonProcessData &process){
    return &process.id;
}

void asign_id(pythonProcessData &process, int _id){
    process.id = _id;
    return;
}

void stop_process(pythonProcessData &process){
    process.run = false;
    if (process.t.joinable()) {
        process.t.join();
        return;
    }
    this_thread::sleep_for(chrono::milliseconds(200));
    if (process.t.joinable()) {
        process.t.join();
    }
    else {
        process.t.detach();
    }
    return;
}

bool is_alive(pythonProcessData &process){
    return process.run;
}

unique_ptr<pythonProcessData> get_process(string _name, args_t _args, int _id){
    return make_unique<pythonProcessData>(_name, _args, _id);
}