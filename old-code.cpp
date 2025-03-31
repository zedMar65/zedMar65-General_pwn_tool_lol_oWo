ProcessInfo StartPythonProcessWithConsole(const string& pythonScriptPath) {
    STARTUPINFOW si = { sizeof(STARTUPINFOW) };
    bool showConsole = true;
    PROCESS_INFORMATION pi = { 0 };
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE hReadPipe = NULL, hWritePipe = NULL, hReadPipeDup = NULL;
    
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

        if (!DuplicateHandle(GetCurrentProcess(), hReadPipe, GetCurrentProcess(), &hReadPipeDup, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
            CloseHandle(hReadPipe);
            CloseHandle(hWritePipe);
            return ProcessInfo(NULL, NULL, NULL);
        }
    } else {
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = hWritePipe;
        si.hStdError = hWritePipe;
    }
    
    wstring wPythonScriptPath = wstring(pythonScriptPath.begin(), pythonScriptPath.end());
    wstring wCommand = L"python " + wPythonScriptPath;

    printf("Command: %ls\n", wCommand.c_str());

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
        if (showConsole) CloseHandle(hReadPipeDup);
        return ProcessInfo(NULL, NULL, NULL);
    }
    DWORD error = GetLastError();
    
    if (showConsole) {
        AttachConsole(pi.dwProcessId);
    }
    
    ResumeThread(pi.hThread);
    
    CloseHandle(hWritePipe);
    
    return ProcessInfo(pi.hProcess, pi.hThread, hReadPipeDup);
}