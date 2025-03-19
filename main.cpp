#include <windows.h>
#include <iostream>
#include <string>
#include <atlstr.h>
#include <vector>
#define UNICODE

using namespace std;

HANDLE StartPythonProcessWithConsole(const string& pythonScriptPath) {
    STARTUPINFOW si = { sizeof(STARTUPINFOW) };
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE hReadPipe, hWritePipe;

    // Create a pipe for the child process's STDOUT.
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        return NULL;
    }

    // Ensure the read handle to the pipe is not inherited.
    if (!SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0)) {
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return NULL;
    }

    // Set up the STARTUPINFO structure.
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = hWritePipe;
    si.hStdError = hWritePipe;

    string command = "python3 " + pythonScriptPath;
    wstring wCommand(command.begin(), command.end());

    // Create the child process.
    if (!CreateProcessW(
            NULL,
            const_cast<wchar_t*>(wCommand.c_str()),
            NULL,
            NULL,
            TRUE,
            0,
            NULL,
            NULL,
            &si,
            &pi)) {
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return NULL;
    }

    // Close the write end of the pipe in the parent process.
    CloseHandle(hWritePipe);

    // Return the handle to the read end of the pipe.
    return hReadPipe;
}

string input(string prompt) {
    cout << prompt;
    string input;
    cin >> input;
    return input;
}

string findMac(string ip){
    cout << "Resolving MAC address for IP: " << ip << endl;
    const string pythonScriptPath = "./assets/get_mac.py --target_ip " + ip;
    HANDLE hReadPipe = StartPythonProcessWithConsole(pythonScriptPath);
    if (hReadPipe == NULL) {
        cout << "Failed to start Python process, while getting mac" << endl;
        return "";
    }
    string result;
    char buffer[128];
    DWORD bytesRead;

    // Read the output from the pipe.
    while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0'; // Null-terminate the buffer.
        result += buffer;
    }

    CloseHandle(hReadPipe);

    // Extract the part inside ||.
    size_t start = result.find("|");
    size_t end = result.rfind("|");
    if (start != string::npos && end != string::npos && start != end) {
        return result.substr(start + 1, end - start - 2);
    }
    cout << "Failed to get MAC address for IP: " << ip << endl;
    return "";
}

struct SeshData
{
    string targetIp = "";
    string targetMac = "";
    string myIp = "";
    string myMac = "";
    string gatewayIp = "";
    string gatewayMac = "";
    string interfaceName = "";
    void print(){
        cout << "--------Session Data--------" << endl;
        cout << "Target IP: " << targetIp << endl;
        cout << "Target MAC: " << targetMac << endl;
        cout << "My IP: " << myIp << endl;
        cout << "My MAC: " << myMac << endl;
        cout << "Gateway IP: " << gatewayIp << endl;
        cout << "Gateway MAC: " << gatewayMac << endl;
        cout << "Interface Name: " << interfaceName << endl;
        cout << "----------------------------" << endl;
    }
    void parseArgs(int argc, char* argv[]) {
        for (int i = 0; i < argc; i++) {
            string arg = argv[i];
            if (arg == "--target-ip") {
                targetIp = argv[i + 1];
            } else if (arg == "--target-mac") {
                targetMac = argv[i + 1];
            } else if (arg == "--my-ip") {
                myIp = argv[i + 1];
            } else if (arg == "--my-mac") {
                myMac = argv[i + 1];
            } else if (arg == "--gateway-ip") {
                gatewayIp = argv[i + 1];
            } else if (arg == "--gateway-mac") {
                gatewayMac = argv[i + 1];
            } else if (arg == "--interface") {
                interfaceName = argv[i + 1];
            }
        }
        fillInMissingData();
        print();
    }
    void fillInMissingData(){
        if (targetIp == "") {
            targetIp = input("Enter the target IP: ");
        }
        if (targetMac == "") {
            targetMac = input("Enter the target MAC: ");
        }
        if (myIp == "") {
            myIp = input("Enter my IP: ");
        }
        if (myMac == "") {
            myMac = input("Enter my MAC: ");
        }
        if (gatewayIp == "") {
            gatewayIp = input("Enter the gateway IP: ");
        }
        if (gatewayMac == "") {
            gatewayMac = input("Enter the gateway MAC: ");
        }
        if (interfaceName == "") {
            interfaceName = input("Enter the interface name: ");
        }
        
        if (targetMac == "f" || targetMac == "find"){
            targetMac = findMac(targetIp);
        }
        if (gatewayMac == "f" || gatewayMac == "find"){
            gatewayMac = findMac(gatewayIp);
        }
        if (myMac == "f" || myMac == "find"){
            myMac = findMac("OWN");
        }
    }
};

SeshData getSeshData(){
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argv == NULL) {
        return SeshData();
    }
    SeshData seshData;
    vector<string> narrowArgs;
    for (int i = 0; i < argc; i++) {
        wstring wideArg = argv[i];
        narrowArgs.push_back(string(wideArg.begin(), wideArg.end()));
    }
    vector<char*> cArgs;
    for (auto& arg : narrowArgs) {
        cArgs.push_back(&arg[0]);
    }
    seshData.parseArgs(argc, cArgs.data());
    return seshData;
}

int InitConsole(){
    // Allocate a console for the main process.
    if (!AllocConsole()) {
        cout << "Failed to allocate console." << endl;
        return -1;
    }

    // Redirect standard input, output, and error to the console.
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);
    cout << "Stawtewd twe uWuifyiwng..." << endl;
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    
    if (InitConsole() == -1) {
        return -1;
    }

    SeshData seshData = getSeshData();
    string temp;
    cin >> temp;


    return 0;
}
