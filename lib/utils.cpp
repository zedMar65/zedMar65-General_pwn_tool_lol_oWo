#include "utils.hpp"
#include "main.hpp"

using namespace std;

unordered_map<string, string> dataList = {
    {"-h", "help"}, {"--help", "help"},
    {"-v", "version"}, {"--version", "version"},
    {"-d", "debug"}, {"--debug", "debug"},
    {"-o", "output"}, {"--output", "output"},
    {"-ipt", "ip_target"}, {"--ip_target", "ip_target"},
    {"-ipr", "ip_router"}, {"--ip_router", "ip_router"},
    {"-ips", "ip_self"}, {"--ip_self", "ip_self"},
    {"-mact", "mac_target"}, {"--mac_target", "mac_target"},
    {"-mipr", "mac_router"}, {"--mac_router", "mac_router"},
    {"-mis", "mac_self"}, {"--mac_self", "mac_self"},
    {"-i", "interface"}, {"--interface", "interface"},
    {"-s", "silent"}, {"--silent", "silent"},
    {"-o", "log"}, {"--output", "log"}
};

void printConsole(string par, int priority) {
    // 1-debug 2-info 3-must print 4-error4
    if (debug) {
        if (priority == 1) {
            cout << "[DEBUG] " << par << endl;
        }
        else if (priority == 2) {
            cout << "[INFO] " << par << endl;
        }
        else if (priority == 3) {
            cout << "[ALERT] " << par << endl;
        }
        else if (priority == 4) {
            cerr << "[ERROR] " << par << endl;
        }
    }
    else if (silent) {
        if (priority == 3) {
            cout << "[ALERT] " << par << endl;
        }
        else if (priority == 4) {
            cerr << "[ERROR] " << par << endl;

        }
    }
    else {
        if (priority == 2) {
            cout << "[INFO] " << par << endl;
        }
        else if (priority == 3) {
            cout << "[ALERT] " << par << endl;
        }
        else if (priority == 4) {
            cerr << "[ERROR] " << par << endl;

        }
    }
}

string getVersion() {
    return "Seshion v0.2.0";
}

unordered_map<string, string> parseArgs(int argc, char* argv[]) {
    unordered_map<string, string> parsedArgs;

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];  // Convert `char*` to `std::string`

        if (dataList.find(arg) != dataList.end()) {
            string key = dataList[arg];

            if (key == "help") {
                for (const auto& p : dataList) {
                    printConsole(p.first + " : " + p.second, 2);
                }
			}
            else if (key == "debug") {
				debug = true;
			}
            else if (key == "silent") {
				silent = true;
			}
            else if (key == "version") {
                printConsole(getVersion(), 2);
            }
            else {
                if (i + 1 < argc) {
                    if (key == "log"){
                        outFile = argv[i+1];
                    }else{
                        parsedArgs[key] = argv[i + 1];
                    }
                    i++;
                }
                else {
                    printConsole("Missing value for argument " + arg, 4);
					parsedArgs[key] = "";  // Assign empty string if no value is provided
                }
            }
        }
        else {
            printConsole("Unknown argument: " + arg, 4);
        }
    }

    return parsedArgs;  // Returning by value (safe)
}

string inputR(string tag) {
	string input;
	cout << tag;
	getline(cin, input);
	return input;
}

void inputThread(string* input, bool* hasInput, bool* run) {
	while (run) {
        if (*hasInput) {
            this_thread::sleep_for(chrono::milliseconds(100));
            continue;
        }
        cout << "\\\\>";
        getline(cin, *input);
		*hasInput = true;
    }
}

vector<string> splitString(string s, string delimiter) {
    vector<string> tokens;
    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens.push_back(s);
    return tokens;
}