#include "utils.hpp"
#include "PScripts.hpp"
#include "main.hpp"
#include "dataStructures.hpp"

typedef std::vector<std::pair<std::string, std::string>> args_t;

using namespace std;

int startTestPythonCode(SeshionData& seshData, bool await) {
	args_t args;
	pythonProcessData &data = *seshData.addProcess("testPythonCode", args);
	getCode(data);
	runCode(data);
	while (data.run && await) {
		this_thread::sleep_for(chrono::milliseconds(100));
	}
	return data.id;
}

int getMacAddress(SeshionData& seshData, string ip, bool await) {
	args_t args;
	args.push_back(pair("IP_TARGET", "'"+ip+"'"));
	pythonProcessData& data = *seshData.addProcess("getMacAddress", args);
	getCode(data);
	runCode(data);
	while (data.run && await) {
		this_thread::sleep_for(chrono::milliseconds(100));
	}
	return data.id;
}

int startArpSpoof(SeshionData& seshData, string targetIp, string targetMac, string routerIp, string routerMac, string iface, bool await) {
	args_t args;
	args.push_back(pair("TARGET_IP", "'" + targetIp + "'"));
	args.push_back(pair("TARGET_MAC", "'" + targetMac + "'"));
	args.push_back(pair("ROUTER_IP", "'" + routerIp + "'"));
	args.push_back(pair("ROUTER_MAC", "'" + routerMac + "'"));
	args.push_back(pair("IFACE", "'" + iface + "'"));
	pythonProcessData& data = *seshData.addProcess("startArpSpoof", args);
	getCode(data);
	runCode(data);
	while (data.run && await) {
		this_thread::sleep_for(chrono::milliseconds(100));
	}
	return data.id;
}

int startForward(SeshionData& seshData, string iface, string ip_target, string mac_target, string mac_self, string mac_router, bool await) {
	if (seshData.getArg("enableForward") != "true") {
		printConsole("Enableing IP forwarding first", 1);
		enableForward(seshData, iface, false);
	}
	
	args_t args;
	args.push_back(pair("IFACE", "'" + iface + "'"));
	args.push_back(pair("IP_TARGET", "'" + ip_target + "'"));
	args.push_back(pair("MAC_TARGET", "'" + mac_target + "'"));
	args.push_back(pair("MAC_SELF", "'" + mac_self + "'"));
	args.push_back(pair("MAC_ROUTER", "'" + mac_router + "'"));
	pythonProcessData& data = *seshData.addProcess("startForward", args);
	getCode(data);
	runCode(data);
	while (data.run && await) {
		this_thread::sleep_for(chrono::milliseconds(100));
	}
	return data.id;
}

int enableForward(SeshionData& seshData, string iface, bool await) {
	if (seshData.getArg("enableForward") == "true" ){
		return -1;
	}

	args_t args;
	args.push_back(pair("IFACE", "'" + iface + "'"));
	pythonProcessData& data = *seshData.addProcess("enableForward", args);
	getCode(data);
	runCode(data);
	while (data.run) {
		this_thread::sleep_for(chrono::milliseconds(100));
	}
	
	while (data.run && await) {
		this_thread::sleep_for(chrono::milliseconds(100));
	}
	
	// might give false positives
	seshData.setArg("enableForward", "true");
	
	return data.id;
}

int disableForward(SeshionData& seshData, string iface, bool await) {
	if (seshData.getArg("enableForward") == "false") {
		return -1;
	}

	args_t args;
	args.push_back(pair("IFACE", "'" + iface + "'"));
	pythonProcessData& data = *seshData.addProcess("disableForward", args);
	getCode(data);
	runCode(data);
	while (data.run) {
		this_thread::sleep_for(chrono::milliseconds(100));
	}
	// might give false positives
	seshData.setArg("enableForward", "false");
	while (data.run && await) {
		this_thread::sleep_for(chrono::milliseconds(100));
	}
	return data.id;
}