#pragma once
#include "main.hpp"
#include "pythonProcess.hpp"
#include <string>
#include <vector>
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
		processes.push_back(start_process(_name, _args, _id));
		return _id;
	}

	pythonProcessData* getProcess(int _id){
		return processes[_id].get();
	}

	void eraseProcess(int _id){
		processes.erase(processes.begin()+_id);
		return;
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
