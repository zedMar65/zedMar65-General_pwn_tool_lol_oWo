#pragma once
#include "utils.hpp"
#include "data.hpp"
#include "PScripts.hpp"
#include "main.hpp"

using namespace std;

struct pythonProcessData {
	int id;
	string name;
	string code;
	string output;
	string error;
	string path;
	bool run;
	thread t;
	vector<pair<string, string>> args;
	pythonProcessData(string _name, vector<pair<string, string>> _args, int _id) {
		this->name = _name;
		this->code = "";
		this->path = "";
		this->output = "";
		this->error = "";
		this->run = true;
		this->t = thread();
		this->args = _args;
		this->id = _id;
	}
	void stop() {
		this->run = false;
		this_thread::sleep_for(chrono::milliseconds(200));
		if (t.joinable()) {
			t.join();
		}
		else {
			t.detach();
		}
		return;
	}
};

class SeshionData {
private:
	unordered_map<string, string> data;
	vector<pythonProcessData*> processes;
	bool run;
	int functionPointer;
	thread InputThread;
public:
	string input = "";
	bool hasInput = false;


	void init(unordered_map<string, string> parsedArgs) {
		for (auto pair : parsedArgs) {
			setArg(pair.first, pair.second);
		}
		this->InputThread = thread(inputThread, &input, &hasInput, &run);
		run = true;
		functionPointer = 0;
	}
	void setArg(string key, string value) {
		data[key] = value;
	}
	void printData() {
		string printData = "\n--------------------------\n";
		for (auto pair : data) {
			printData += pair.first + " : " + pair.second + "\n";
		}
		printData += "-------Process Data-------\n";
		for (int i = 0; i < processes.size(); i++) {
			printData += "Id: " + to_string(i) + " | Path: " + processes[i]->path + " | Running: " + to_string(processes[i]->run) + "\n";
		}
		printData += "--------------------------\n";
		printConsole(printData, 2);
	}
	string getArg(string key) {
		if (data.find(key) != data.end()) {
			return data[key];
		}
		else {
			printConsole("Key not found: " + key, 4);
			return "";
		}
	}
	bool* state() {
		return &run;
	}
	void setState(bool state) {
		run = state;
	}
	void exitProgram() {
		run = false;
		printConsole("Starting exiting process", 1);
		InputThread.detach();
		printConsole("Input thread joined", 1);
		for(int i = 0; i < processes.size(); i++){
			stop_proces(getProcess(i));
		}
		cleanProcesses();
		printConsole("Closed processes", 1);
		printConsole("Exiting program", 2);
		exit(0);
	}
	void basicInput() {
		data["interface"] = inputR("Interface: ");
		data["ip_self"] = inputR("IP self: ");
		data["ip_target"] = inputR("IP target: ");
		data["ip_router"] = inputR("IP router: ");
		data["mac_self"] = inputR("MAC self: ");
		data["mac_target"] = inputR("MAC target: ");
		data["mac_router"] = inputR("MAC router: ");
		fillMacks();
	}
	void fillMacks() {
		if (data.find("mac_self") == data.end() || data["mac_self"] == "") {
			data["mac_self"] = *(get_data(getProcess(getMacAddress(*this, "self"))));
		}
		if (data.find("mac_target") == data.end() || data["mac_target"] == "") {
			data["mac_target"] = *(get_data(getProcess(getMacAddress(*this, "ip_target"))));
		}
		if (data.find("mac_router") == data.end() || data["mac_router"] == "") {
			data["mac_router"] = *(get_data(getProcess(getMacAddress(*this, "ip_router"))));
		}
	}
	void eraseArg(string key) {
		if (data.find(key) != data.end()) {
			data.erase(key);
		}
		else {
			printConsole("Key not found: " + key, 4);
		}
	}
	pythonProcessData* addProcess(string name, vector<pair<string, string>> args) {
		
		pythonProcessData* data = new pythonProcessData(name, args, processes.size());

		processes.push_back(data);
		return data;
	}
	pythonProcessData* getProcess(int id){
		if(processes.size() >= id){
			printConsole("Maching id isn't available", 3);
		}
		return processes[id];
	}
	void cleanProcesses() {
		for (int i = 0; i < processes.size(); i++) {
			if (!processes[i]->run) {
				printConsole("Erasing Process: " + to_string(i), 1);
				processes.erase(processes.begin()+i);
				i--;
			}
		}
	}
	void resignIds(){
		for(int i = 0; i < processes.size(); i++){
			asign_id(processes[i], i);
			
		}
	}
};
