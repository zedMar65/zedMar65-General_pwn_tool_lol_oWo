#pragma once
#include "main.hpp"
#include "pythonProcess.hpp"
#include <memory>
#include <string>
#include <unordered_map>


using namespace std;

class SessionData {
private:

	unordered_map<string, string> data;
	vector<unique_ptr<pythonProcessData>> processes;	
public:

	void setArg(string key, string value) {
		data[key] = value;
	}

	string getArg(string key) {
		return data[key];
	}

	void eraseArg(string key) {
		data.erase(key);
	}

	int startProcess(string _name, args_t _args) {
		int _id = processes.size();
		processes.push_back(get_process(_name, _args, _id));
		return _id;
	}

	pythonProcessData* getProcess(int _id){
		return processes[_id].get();
	}

	vector<string> getAllKeys(){
		vector<string> keys;
		for(auto& pair : data){
			keys.push_back(pair.first);
		}
		return keys;
	}
	
	int getProcessLen(){
		return processes.size();
	}

	bool evalKey(string _key){
		if(data.find(_key) == data.end()){
			return false;
		}
		return true;
	}
};
