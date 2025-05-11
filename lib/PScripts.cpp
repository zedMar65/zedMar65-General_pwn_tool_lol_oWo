#include "dataStructures.hpp"
#include "utils.hpp"
#include "main.hpp"
#include "PScripts.hpp"

void runCode(pythonProcessData& data) {
	printConsole("Running code: " + data.name, 1);
	data.t = thread([&]() {
	});
}

void getCode(pythonProcessData& data) {
	string code;
	data.path = "./temp_script_" + data.name + "_" + to_string((int)time(0)) + ".py";
	if (data.name == "testPythonCode") {
		code = testPythonCode;
	}
	else if (data.name == "getMacAddress") {
		code = getMacAddressCode;
	}
	else if (data.name == "startArpSpoof") {
		code = startArpSpoofCode;
	}
	else if (data.name == "startForward") {
		code = startForwardCode;
	}
	else if (data.name == "enableForward") {
		code = allowForwardCode;
	}
	else if (data.name == "disableForward") {
		code = dissableForwardCode;
	}
	else {
		printConsole("Error: No code found for " + data.name, 4);
		data.code = "";
		return;
	}
	for (int i = 0; i < data.args.size(); i++) {
		code = data.args[i].first + " = " + data.args[i].second + "\n"+code;
	}
	data.code = (code);

	printConsole("Running python script: " + data.path, 1);
	ofstream scriptFile(data.path, ios::trunc);
	scriptFile << data.code;
	scriptFile.close();
}