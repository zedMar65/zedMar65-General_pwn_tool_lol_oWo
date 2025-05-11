#include "data.hpp"
#include "dataStructures.hpp"

string* get_err(pythonProcessData &process){
    return &process.error;
}

string* get_data(pythonProcessData &process){
    return &process.output;
}

int* get_id(pythonProcessData &process){
    return &process.id;
}

void stop_proces(pythonProcessData &process){
    process.stop();
    return;
}

void asign_id(pythonProcessData &process, int _id){
    process.id = _id;
    return;
}