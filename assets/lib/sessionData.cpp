#include "sessionData.hpp"
#include "sessionData_internal.hpp"
#include <memory>
using namespace std;

unique_ptr<SessionData> start_session(){
    unique_ptr<SessionData> seshData = unique_ptr<SessionData>();
    set_arg(*seshData, "run", "true");
    return seshData;
}

string session_status(SessionData &seshData){
    string data = "variables: [\n";
    vector<string> keys = seshData.getAllKeys();
    for(int i = 0; i < keys.size(); i++){
        data +="{\"key\": \"" + keys[i] + "\"" + ", \"value\": \"" + get_arg(seshData, keys[i]) + "\"}";
        if (i != keys.size()-1){
            data += ",";
        } 
    }

    data += "],\nvariables: [\n";
    for(int i = 0; i < keys.size(); i++){
        data +="{\"key\": \"" + keys[i] + "\"" + ", \"value\": \"" + get_arg(seshData, keys[i]) + "\"},"; 
    }
    data += "]}";
    return data;
}

void set_arg(SessionData &seshData, string _key, string _value){
}
string get_arg(SessionData &seshData, string _key);
pythonProcessData& get_process(SessionData &seshData, int _id);
int new_process(SessionData &seshData, string _name, args_t args);
void erase_process(SessionData &seshData, int _id);
void clear_processes(SessionData &seshData);
void quit_session(SessionData &seshData);