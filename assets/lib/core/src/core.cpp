#include "main.hpp"
#include "ui.hpp"
#include <string>
#include <unordered_map>
#include <utility>

std::unordered_map<std::string, std::string> const_bank = {
    {"SOME_CONST", "$ip_var"}
};

std::unordered_map<std::string, std::pair<args_t, std::string>> code_bank = {
    {
        "STD_REDIRECT",
        {
            {},
            R"(
import sys
import io

stdout_capture = io.StringIO()
stderr_capture = io.StringIO()

sys.stdout = stdout_capture
sys.stderr = stderr_capture
)"
        }
    },
    {
        "TEST_CODE",
        {
            {
                {"TARGET_IP", "SOME_CONST"}
            },
            "print(TARGET_IP)"
        }
    }
};

std::string get_const(std::string _key){
    if (const_bank.find(_key)!=const_bank.end()){
        return const_bank[_key];
    }
    log("Const does not exist", ERROR);
    return "";
}

void _mod_const(std::string _key, std::string _value){
    const_bank[_key] = _value;
}

std::pair<args_t, std::string> get_code(std::string _key){
    if (code_bank.find(_key)!=code_bank.end()){
        return code_bank[_key];
    }
    log("Code does not exist", ERROR);
    return {{}, ""};
}

void _mod_code(std::string _key, std::pair<args_t, std::string> _value){
    code_bank[_key] = _value;
}