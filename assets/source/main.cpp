#include "sessionData.hpp"
#include <iostream>

int main(){
    Session session;
    // var "run": "true" is set by default...
    session.set_arg("run_pointer", "$run");
    cout << session.session_status();
    cout << "\nrun_pointer value: " + session.get_arg("run_pointer");
    session.set_arg("$run_pointer", "CustomName");
    session.new_process("$true", args_t());
    cout << session.session_status();
    return 0;
}