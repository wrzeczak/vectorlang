// nob.c
#define NOB_IMPLEMENTATION
#define NOB_NO_ECHO
#include "nob.h"

#define EXEC "wvcomp"
#define INPUT_FILE "example1.wvlang"

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, "gcc", "-Wall", "-Wextra", "-std=c23", "-g", "-DDEBUG", "-o", EXEC, "main.c");
    if(!nob_cmd_run(&cmd)) return 1;
    // ./nob build = do not run executable
    // ./nob debug = run gdb
    if(argc >= 2) {
        if(strcmp("debug", argv[1]) == 0) {
            nob_log(NOB_INFO, "Building and running GDB...");
            Nob_Cmd dbg = {0};
            nob_cmd_append(&dbg, "gdb", EXEC);
            if(!nob_cmd_run(&dbg)) return 1;
        } else if(strcmp("build", argv[1]) == 0) {
            return 0;
        } else {
            nob_log(NOB_INFO, "Passing \"%s\" to "EXEC" at runtime...");
            Nob_Cmd exe = {0};
            nob_cmd_append(&exe, "./"EXEC, argv[1]);
            if(!nob_cmd_run(&exe)) return 1;
        }
    } else { // same as above
        // nob_log(NOB_INFO, "Building without execution...");
        Nob_Cmd exe = {0};
        nob_cmd_append(&exe, "./"EXEC, INPUT_FILE);
        if(!nob_cmd_run(&exe)) return 1;
    }
    return 0;
}