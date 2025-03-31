#include <windows.h>
#include <iostream>
#include <string>
#include <atlstr.h>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <thread>
#include <atomic>
#define UNICODE

using namespace std;


struct SeshData;

void print(SeshData& seshData, string str){
    cout << str << endl;
}

struct ProcessInfo {
    ProcessInfo(HANDLE hProcess, HANDLE hThread, HANDLE hReadPipe)
        : hProcess(hProcess), hThread(hThread), hReadPipe(hReadPipe) {}
    HANDLE hProcess;
    HANDLE hThread;
    HANDLE hReadPipe;
};

ProcessInfo StartPythonProcess(const string& pythonScriptPath) {
    STARTUPINFOW si = { sizeof(STARTUPINFOW) };
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE hReadPipe, hWritePipe;

    // Create a pipe for the child process's STDOUT.
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        return {NULL, NULL, NULL};
    }

    // Ensure the read handle to the pipe is not inherited.
    if (!SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0)) {
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return {NULL, NULL, NULL};
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
        return {NULL, NULL, NULL};
    }

    // Close the write end of the pipe in the parent process.
    CloseHandle(hWritePipe);

    // Return the handle to the read end of the pipe.
    return {pi.hProcess, pi.hThread, hReadPipe};
}

ProcessInfo StartPythonProcessWithConsole(const string& pythonScriptPath) {
    STARTUPINFOW si = { sizeof(STARTUPINFOW) };
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE hReadPipe, hWritePipe;

    // Create a pipe for the child process's STDOUT.
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        return {NULL, NULL, NULL};
    }

    // Ensure the read handle to the pipe is not inherited.
    if (!SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0)) {
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return {NULL, NULL, NULL};
    }

    AllocConsole();
    

    string command = "python " + pythonScriptPath;
    wstring wCommand(command.begin(), command.end());

    // Create the child process.
    si.hStdError = hWritePipe;
    si.dwFlags |= STARTF_USESTDHANDLES;
    if (!CreateProcessW(
            NULL,
            const_cast<wchar_t*>(wCommand.c_str()),
            NULL,
            NULL,
            TRUE,
            CREATE_NEW_CONSOLE,
            NULL,
            NULL,
            &si,
            &pi)) {
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return {NULL, NULL, NULL};
    }

    CloseHandle(hWritePipe);

    AttachConsole(pi.dwProcessId);
    // Close the original read end of the pipe in the parent process.
    ResumeThread(pi.hThread);

    // Return the handle to the read end of the pipe.
    return ProcessInfo(pi.hProcess, pi.hThread, hReadPipe);
}

string input(string prompt) {
    cout << prompt;
    getline(cin, prompt);
    return prompt;
}

string findMac(string ip){
    const string pythonScriptPath = "./assets/get_mac.py --target_ip " + ip;
    ProcessInfo tempInfo = (StartPythonProcess(pythonScriptPath));
    if (tempInfo.hReadPipe == NULL) {
        return "";
    }
    string result;
    char buffer[128];
    DWORD bytesRead;

    while (ReadFile(tempInfo.hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        result += buffer;
    }

    CloseHandle(tempInfo.hReadPipe);

    size_t start = result.find("|");
    size_t end = result.rfind("|");
    if (start != string::npos && end != string::npos && start != end) {
        return result.substr(start + 1, end - start - 1);
    }
    return "";
}

void inputListener(atomic<bool>& has_input, string& command){
    while (true) {
        if (has_input) {
            this_thread::sleep_for(chrono::milliseconds(100));
            continue;
        }
        string inputLine;
        inputLine = input("\\\\> ");
        command = inputLine;
        has_input = true;
    }
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
    string command = "";
    atomic<bool> has_input = false;
    thread inputThread;
    SeshData* seshData = NULL;
    bool debug = false;
    ProcessInfo serverHandle = { NULL, NULL, NULL };
    unordered_map<string, vector<ProcessInfo>> otherProcesses;
    void printSesh(){
        print(seshData[0], "--------Session Data--------");
        print(seshData[0], "Target IP: " + targetIp);
        print(seshData[0], "Target MAC: " + targetMac);
        print(seshData[0], "My IP: " + myIp);
        print(seshData[0], "My MAC: " + myMac);
        print(seshData[0], "Gateway IP: " + gatewayIp);
        print(seshData[0], "Gateway MAC: " + gatewayMac);
        print(seshData[0], "Interface Name: " + interfaceName);
        print(seshData[0], "Server Handle: " + to_string((uintptr_t)serverHandle.hProcess));
        print(seshData[0], "-------Other Processes------");
        for(auto& [key, value] : otherProcesses){
            print(seshData[0], key + ":");
            for(int i = 0; i < value.size(); i++){
                print(seshData[0], "    " + to_string(i) + ": " + to_string((uintptr_t)value[i].hProcess));
            }
        }
        print(seshData[0], "----------------------------");
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
            } else if (arg == "--debug") {
                debug = true;
            } else if (arg == "--help") {
                print(seshData[0], "Options:");
                print(seshData[0], "  --target-ip <ip>");
                print(seshData[0], "  --target-mac <mac>");
                print(seshData[0], "  --my-ip <ip>");
                print(seshData[0], "  --my-mac <mac>");
                print(seshData[0], "  --gateway-ip <ip>");
                print(seshData[0], "  --gateway-mac <mac>");
                print(seshData[0], "  --interface <name>");
                print(seshData[0], "  --debug");
            }
        }
        fillInMissingData();
        printSesh();
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

SeshData* getSeshData(){
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argv == NULL) {
        SeshData seshData = SeshData();
        seshData.seshData = &seshData;
        return &seshData;
    }
    static SeshData seshData;
    seshData.seshData = &seshData;
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
    seshData.inputThread = thread(inputListener, ref(seshData.has_input), ref(seshData.command));
    return &seshData;
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

bool startServer(SeshData *seshData){
    seshData->serverHandle = { NULL, NULL, NULL };
    seshData->serverHandle = StartPythonProcessWithConsole("./assets/server.py");
    if (seshData->serverHandle.hProcess == NULL) {
        print(seshData[0], "Failed to start server. Error: "+GetLastError());
        return false;
    }

    string result;
    char buffer[128];
    DWORD bytesRead;
    string lineBuffer;
    return true;
}

bool closeProcess(SeshData *seshData, ProcessInfo *processInfo){
    DWORD exitCode;
    if ((GetExitCodeProcess(processInfo->hProcess, &exitCode) && exitCode != STILL_ACTIVE) || processInfo->hProcess == NULL || WaitForSingleObject(processInfo->hProcess, 0) == WAIT_OBJECT_0){
        if (seshData->debug) {
            print(seshData[0], "Process already exited.");
        }
        return true;
    }
    print(seshData[0], "OK");
    if (TerminateProcess(processInfo->hProcess, 0)) {
        print(seshData[0], "Terminated process successfully.");
    } else {
        print(seshData[0], "Failed to terminate process. Error: "+GetLastError());
        return false;
    }
    CloseHandle(processInfo->hThread);
    CloseHandle(processInfo->hReadPipe);
    CloseHandle(processInfo->hProcess);
    
    return true;
}

bool cleanup(SeshData *seshData){
    HANDLE proc = StartPythonProcess("./assets/cleanup.py --interface " + seshData->interfaceName+" --target_ip "+seshData->targetIp+" --target_mac "+seshData->targetMac+" --gateway_ip "+seshData->gatewayIp+" --gateway_mac "+seshData->gatewayMac).hProcess;
    if (proc == NULL) {
        print(seshData[0], "Failed to start cleanup process.");
        return false;
    }
    print(seshData[0], "Cleanup process started successfully.");
    WaitForSingleObject(proc, 5000);
    CloseHandle(proc);
}

void parseCommand(string commandLine, SeshData *seshData, bool *run){
    if (seshData->debug) {
        print(seshData[0], "Debug: Parsing command: "+commandLine+"\n");
    }
    string tempCom = "";
    commandLine += (char)00;
    vector<string> flags;
    SeshData &seshDataRef = *seshData;
    bool isFlag = false;
    string log = "0";
    string command = "";
    for(char &c : commandLine){
        c = tolower(c);
        if ((!isFlag && c == '-') || (!isFlag &&(c == '\0'))){
            command = tempCom;
            tempCom = "";
            if (c == '\0') break;
            command.pop_back();
            isFlag = true;
        }
        if(isFlag){
            if(c == ' ' || c == '\0'){
                flags.push_back(tempCom);
                tempCom = "";
            }else{
                tempCom += c;
            }
        }else{
            tempCom += c;
        }
    }
    for(int i = 0; i < flags.size(); i++){
        if(flags[i] == "-l" || flags[i] == "-log"){
            log = "1";
        }
    }
    
    
    if (command == "exit") {
        *run = false;
    }else if (command == "debug"){
        if (seshDataRef.debug) {
            seshDataRef.debug = false;
            print(seshData[0], "Debug mode disabled.");
        } else {
            seshDataRef.debug = true;
            print(seshData[0], "Debug mode enabled.");
        }
    }
    else if(command == "stop server"){
        closeProcess(seshData, &seshDataRef.serverHandle);
    }else if(command == "start server"){
        startServer(seshData);
    }else if(command == "print"){
        seshDataRef.printSesh();
    }else if(command == "start dns"){
        seshDataRef.otherProcesses["dns"].push_back(StartPythonProcessWithConsole("./assets/dnsspoof.py --target_ip " + seshDataRef.targetIp + " --target_mac " + seshDataRef.targetMac + " --gateway_ip " + seshDataRef.gatewayIp + " --gateway_mac " + seshDataRef.gatewayMac + " --interface " + seshDataRef.interfaceName + " --log " + log));
    }else if(command == "stop dns"){
        if (seshDataRef.otherProcesses["dns"].empty()){
            print(seshData[0], "No DNS process to stop.");
            return;
        }
        closeProcess(seshData, &seshDataRef.otherProcesses["dns"].back());
        seshDataRef.otherProcesses["dns"].pop_back();
    }else if(command == "start arp"){
        seshDataRef.otherProcesses["arp"].push_back(StartPythonProcessWithConsole("./assets/arpspoof.py --target_ip " + seshDataRef.targetIp + " --target_mac " + seshDataRef.targetMac + " --gateway_ip " + seshDataRef.gatewayIp + " --gateway_mac " + seshDataRef.gatewayMac + " --interface " + seshDataRef.interfaceName + " --log " + log));
    }else if(command == "stop arp"){
        if (seshDataRef.otherProcesses["arp"].empty()){
            print(seshData[0], "No ARP process to stop.");
            return;
        }
        closeProcess(seshData, &seshDataRef.otherProcesses["arp"].back());
        seshDataRef.otherProcesses["arp"].pop_back();
    }else if(command == "start forward"){
        seshDataRef.otherProcesses["forward"].push_back(StartPythonProcessWithConsole("./assets/forward.py --target_ip " + seshDataRef.targetIp + " --target_mac " + seshDataRef.targetMac + " --gateway_ip " + seshDataRef.gatewayIp + " --gateway_mac " + seshDataRef.gatewayMac + " --interface " + seshDataRef.interfaceName + " --log " + log));
    }else if(command == "stop forward"){
        if (seshDataRef.otherProcesses["forward"].empty()){
            print(seshData[0], "No forward process to stop.");
            return;
        }
        closeProcess(seshData, &seshDataRef.otherProcesses["forward"].back());
        seshDataRef.otherProcesses["forward"].pop_back();
    }else if (command == "free processes"){
        for(auto& [key, value] : seshDataRef.otherProcesses){
            for(int i = 0; i < value.size(); i++){
                closeProcess(seshData, &value[i]);
            }
            seshDataRef.otherProcesses[key].clear();
    }
    }else if (command == "clear"){
        system("cls");
    }else if(command == "cleanup"){
        cleanup(seshData);
    }else if(command == "log"){
        ifstream logFile("./assets/data.log");
        if (logFile.is_open()){
            string line;
            while (getline(logFile, line)){
                print(seshData[0], line);
            }
            logFile.close();
        }
    }else if (command == "set vars"){
        seshDataRef.fillInMissingData();
    }else if (command == "find mac"){
        string ip = input("Enter the IP to find the MAC address for: ");
        print(seshData[0], "MAC address for IP: " + ip +" is: " + findMac(ip));
    }else if (command == "reset vars"){
        for(auto& [key, value] : seshDataRef.otherProcesses){
            for(int i = 0; i < value.size(); i++){
                closeProcess(seshData, &value[i]);
            }
        }
        seshDataRef.otherProcesses.clear();
        seshDataRef.targetIp = "";
        seshDataRef.targetMac = "";
        seshDataRef.myIp = "";
        seshDataRef.myMac = "";
        seshDataRef.gatewayIp = "";
        seshDataRef.gatewayMac = "";
        seshDataRef.interfaceName = "";
    }else if(command == "help"){
        print(seshData[0], "Commands:");
        print(seshData[0], "  exit");
        print(seshData[0], "  debug");
        print(seshData[0], "  start server");
        print(seshData[0], "  stop server");
        print(seshData[0], "  start dns");
        print(seshData[0], "  stop dns");
        print(seshData[0], "  start arp");
        print(seshData[0], "  stop arp");
        print(seshData[0], "  start forward");
        print(seshData[0], "  stop forward");
        print(seshData[0], "  cleanup");
        print(seshData[0], "  clear");
        print(seshData[0], "  print");
        print(seshData[0], "  find mac");
        print(seshData[0], "  set vars");
        print(seshData[0], "  reset vars");
        print(seshData[0], "  log");
        print(seshData[0], "  free processes");
        print(seshData[0], "  help");
        print(seshData[0], "  exit");
        print(seshData[0], "  debug");
    }
    else{
        print(seshData[0], "Unknown command: [" + command + "]");
    }
}

int mainLoop(SeshData *seshData, bool *run){
    seshData->has_input = true;
    if (seshData->has_input && seshData->command != "") {
        parseCommand(seshData->command, seshData, run);
        seshData->command = "";
    }
    
    
    for(auto& [key, value] : seshData->otherProcesses){
        for(int j = 0; j  < seshData->otherProcesses[key].size(); j++){
            DWORD exitCode;
            if ((GetExitCodeProcess(seshData->otherProcesses[key][j].hProcess, &exitCode) && exitCode != STILL_ACTIVE) || seshData->otherProcesses[key][j].hProcess == NULL || WaitForSingleObject(seshData->otherProcesses[key][j].hProcess, 0) == WAIT_OBJECT_0){
                print(seshData[0], "\nProcess " + key + "-" + to_string(j) + " exited.");
                closeProcess(seshData, &seshData->otherProcesses[key][j]);
                seshData->otherProcesses[key].erase(seshData->otherProcesses[key].begin() + j);
                j--;
                continue;
            }
            if (seshData->otherProcesses[key].empty()){
                seshData->otherProcesses.erase(key);
            }
            if (seshData->debug) {
                char ch;
                DWORD bytesRead;
                std::string line;
                if (seshData->debug) {
                    print(seshData[0], "Debug: Reading from process " + key + "-" + to_string(j) + ": " + to_string((uintptr_t)seshData->otherProcesses[key][j].hReadPipe));
                }
                if (seshData->otherProcesses[key][j].hReadPipe == NULL) {
                    print(seshData[0], "Debug: Read pipe is NULL for process " + key + "-" + to_string(j));
                    continue;
                }
                while (ReadFile(seshData->otherProcesses[key][j].hReadPipe, &ch, 1, &bytesRead, NULL) && bytesRead > 0) {
                    if (ch == '\0' || ((GetExitCodeProcess(seshData->otherProcesses[key][j].hProcess, &exitCode) && exitCode != STILL_ACTIVE) || seshData->otherProcesses[key][j].hProcess == NULL || WaitForSingleObject(seshData->otherProcesses[key][j].hProcess, 0) == WAIT_OBJECT_0)){
                        break;
                    }
                    if (ch == '\n') { // End of line detected
                        print(seshData[0], key +"-" + to_string(j) + ": " + line);
                        line.clear(); // Reset for the next line
                    } else {
                        line += ch;
                    }
                }
                if (!line.empty()) {
                    print(seshData[0], key +"-"+ to_string(j) + ": "+ line);
                }
            }

            
            
        }
    }
    seshData->has_input = false;
    

    return 0;
}

void exitMain(SeshData &seshData){
    input("Press ant key to exit...\n");
    seshData.inputThread.detach();
    cleanup(&seshData);
    closeProcess(&seshData, &seshData.serverHandle);
    for(auto& [key, value] : seshData.otherProcesses){
        for(int i = 0; i < value.size(); i++){
            closeProcess(&seshData, &value[i]);
        }
    }
    FreeConsole();
    ExitProcess(0);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    bool run = true;
    if (InitConsole() == -1) {
        exitMain(SeshData());
    }

    SeshData* seshData = getSeshData();

    while (run) {
        mainLoop(seshData, &run);
        Sleep(0.1);
    }
    
    exitMain(seshData[0]);

    return 0;
}
