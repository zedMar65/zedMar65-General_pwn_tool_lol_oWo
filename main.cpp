#include <windows.h>
#include <iostream>
#include <string>
#include <atlstr.h>
#include <vector>
#define UNICODE

using namespace std;

struct ProcessInfo {
    ProcessInfo(HANDLE hProcess, HANDLE hThread, HANDLE hReadPipe)
        : hProcess(hProcess), hThread(hThread), hReadPipe(hReadPipe) {}
    HANDLE hProcess;
    HANDLE hThread;
    HANDLE hReadPipe;
};

HANDLE StartPythonProcess(const string& pythonScriptPath) {
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

ProcessInfo StartPythonProcessWithConsole(const string& pythonScriptPath) {
    STARTUPINFOW si = { sizeof(STARTUPINFOW) };
    bool showConsole = true;
    PROCESS_INFORMATION pi = { 0 };
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE hReadPipe = NULL, hWritePipe = NULL, hWritePipeDup = NULL;
    
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        return ProcessInfo(NULL, NULL, NULL);
    }
    
    if (!SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0)) {
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return ProcessInfo(NULL, NULL, NULL);
    }
    
    if (showConsole) {
        AllocConsole();
    
        if (!DuplicateHandle(GetCurrentProcess(), hWritePipe, GetCurrentProcess(), &hWritePipeDup, 0, TRUE, DUPLICATE_SAME_ACCESS)) {
            CloseHandle(hReadPipe);
            CloseHandle(hWritePipe);
            return ProcessInfo(NULL, NULL, NULL);
        }
    
        SetStdHandle(STD_OUTPUT_HANDLE, hWritePipeDup);
        SetStdHandle(STD_ERROR_HANDLE, hWritePipeDup);
    } else {
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = hWritePipe;
        si.hStdError = hWritePipe;
    }
    
    wstring wPythonScriptPath = wstring(pythonScriptPath.begin(), pythonScriptPath.end());
    wstring wCommand = L"python \"" + wPythonScriptPath + L"\"";
    
    DWORD consoleParams = showConsole ? CREATE_NEW_CONSOLE : CREATE_NO_WINDOW | CREATE_SUSPENDED;
    
    if (!CreateProcessW(
            NULL,
            const_cast<wchar_t*>(wCommand.c_str()),
            NULL,
            NULL,
            TRUE,
            consoleParams,
            NULL,
            NULL,
            &si,
            &pi)) {
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        if (showConsole) CloseHandle(hWritePipeDup);
        return ProcessInfo(NULL, NULL, NULL);
    }
    
    if (showConsole) {
        AttachConsole(pi.dwProcessId);
    }
    
    ResumeThread(pi.hThread);
    
    CloseHandle(hWritePipe);
    if (showConsole) CloseHandle(hWritePipeDup);
    
    return ProcessInfo(pi.hProcess, pi.hThread, hReadPipe);
}

string input(string prompt) {
    cout << prompt;
    getline(cin, prompt);
    return prompt;
}

string findMac(string ip){
    cout << "Resolving MAC address for IP: " << ip << endl;
    const string pythonScriptPath = "./assets/get_mac.py --target_ip " + ip;
    HANDLE hReadPipe = StartPythonProcess(pythonScriptPath);
    if (hReadPipe == NULL) {
        cout << "Failed to start Python process, while getting mac" << endl;
        return "";
    }
    string result;
    char buffer[128];
    DWORD bytesRead;

    while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        result += buffer;
    }

    CloseHandle(hReadPipe);

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
    ProcessInfo serverHandle = { NULL, NULL, NULL };
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
    if (!AllocConsole()) {
        cout << "Failed to allocate console." << endl;
        return -1;
    }

    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);
    cout << "Stawtewd twe uWuifyiwng..." << endl;
    return 0;
}

int mainLoop(SeshData seshData, bool *run){
    string command = input("\\\\> ");
    if (command == "exit") {
        *run = false;
    }
    
    return 0;
}

void exit(SeshData seshData){
    input("Press ant key to exit...");
    int num = TerminateProcess(seshData.serverHandle.hProcess, 0);
    cout << "Terminated server process with code: " << num << endl;
    CloseHandle(seshData.serverHandle.hThread);
    FreeConsole();
    ExitProcess(0);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    bool run = true;
    if (InitConsole() == -1) {
        exit(SeshData());
    }

    SeshData seshData = getSeshData();
    if (seshData.targetIp == "" || seshData.targetMac == "" || seshData.myIp == "" || seshData.myMac == "" || seshData.gatewayIp == "" || seshData.gatewayMac == "" || seshData.interfaceName == "") {
        cout << "Failed to get session data." << endl;
        exit(seshData);
    }
    seshData.serverHandle = StartPythonProcessWithConsole("./assets/server.py");
    if (seshData.serverHandle.hProcess == NULL) {
        cout << "Failed to start server." << endl;
        exit(seshData);
    }

    while (run) {
        mainLoop(seshData, &run);
        Sleep(0.1);
    }
    
    exit(seshData);

    return 0;
}
