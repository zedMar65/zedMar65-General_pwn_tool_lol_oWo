#pragma once
#include <string>
#include <thread>
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
	args_t args;
	pythonProcessData(string _name, args_t _args, int _id) {
		name = _name;
		code = "";
		path = "";
		output = "";
		error = "";
		run = true;
		t = thread();
		args = _args;
		id = _id;
	}
};