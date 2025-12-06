#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "../include/libs.h"

#define BUFFER_SIZE 4096

typedef float (*e_func_t)(int);
typedef char* (*convert_func_t)(int);

enum ErrorCode {
    OK = 0,
    ER_DLOPEN = -1,
    ER_DLSYM = -2,
};

enum CurrentLib {
    FIRST = 0,
    SECOND = 1,
};

enum ErrorCode command_0(const char** LIB_NAMES, void** library, int* current_lib, e_func_t* e_func, convert_func_t* convert_func) {
    char buffer[BUFFER_SIZE];
    
    if (*library) {
        dlclose(*library);
        *library = NULL;
    }

    *current_lib = (*current_lib == FIRST) ? SECOND : FIRST;

    *library = dlopen(LIB_NAMES[*current_lib], RTLD_NOW);
    
    if (!(*library)) {
        int len = snprintf(buffer, BUFFER_SIZE, "error switching libs: %s\n", dlerror());
        write(STDERR_FILENO, buffer, len);
        return ER_DLOPEN;
    }

    dlerror();

    *e_func = (e_func_t)dlsym(*library, "e");
    if (!*e_func) {
        int len = snprintf(buffer, BUFFER_SIZE, "error finding 'e': %s\n", dlerror());
        write(STDERR_FILENO, buffer, len);
        return ER_DLSYM;
    }

    *convert_func = (convert_func_t)dlsym(*library, "convert");
    if (!*convert_func) {
        int len = snprintf(buffer, BUFFER_SIZE, "error finding 'convert': %s\n", dlerror());
        write(STDERR_FILENO, buffer, len);
        return ER_DLSYM;
    }

    int len = snprintf(buffer, BUFFER_SIZE, "switched to library: %s\n", LIB_NAMES[*current_lib]);
    write(STDOUT_FILENO, buffer, len);

    return OK;
}

void command_1(e_func_t e_func) {
    if (!e_func) return;

    char* arg1 = strtok(NULL, " \t\n");
    char buffer[BUFFER_SIZE];
    int len = 0;

    if (arg1) {
        int x = atoi(arg1);
        float res = e_func(x);
        len = snprintf(buffer, BUFFER_SIZE, "e(%d) result: %.6f\n", x, res);
        write(STDOUT_FILENO, buffer, len);
    } else {
        const char msg[] = "error: missing argument for command 1\n";
        write(STDERR_FILENO, msg, sizeof(msg)-1);
    }
}

void command_2(convert_func_t convert_func) {
    if (!convert_func) return;

    char* arg1 = strtok(NULL, " \t\n");
    char buffer[BUFFER_SIZE];
    int len = 0;

    if (arg1) {
        int x = atoi(arg1);
        char* res = convert_func(x);
        
        if (res) {
            len = snprintf(buffer, BUFFER_SIZE, "convert(%d) result: %s\n", x, res);
            write(STDOUT_FILENO, buffer, len);
            free(res);
        } else {
            const char msg[] = "error: memory alloc\n";
            write(STDERR_FILENO, msg, sizeof(msg)-1);
        }
    } else {
        const char msg[] = "error: missing argument for command 2\n";
        write(STDERR_FILENO, msg, sizeof(msg)-1);
    }
}

int main() {
    const char* LIB_NAMES[] = {"./lib1.so", "./lib2.so"};
    int current_lib = FIRST;

    e_func_t e_func = NULL;
    convert_func_t convert_func = NULL;
    void* library = NULL;
    char buffer[BUFFER_SIZE];

    library = dlopen(LIB_NAMES[current_lib], RTLD_NOW);
    if (!library) {
        int len = snprintf(buffer, BUFFER_SIZE, "error loading initial lib: %s\n", dlerror());
        write(STDERR_FILENO, buffer, len);
        return ER_DLOPEN;
    }

    e_func = (e_func_t)dlsym(library, "e");
    convert_func = (convert_func_t)dlsym(library, "convert");

    if (!e_func || !convert_func) {
        const char msg[] = "error: functions not found in library\n";
        write(STDERR_FILENO, msg, sizeof(msg)-1);
        return ER_DLSYM;
    }

    {
        const char *msg = "dynamic program.\ncommands: 0 - switch; 1 - e_func; 2 - convert\n> ";
        write(STDOUT_FILENO, msg, strlen(msg));
    }

    int bytes_read;
    while ((bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = '\0';
        char *token = strtok(buffer, " \t\n");
        if (!token) {
             write(STDOUT_FILENO, "> ", 2);
             continue;
        }

        int cmd = atoi(token);
        switch (cmd) {
            case 0:
                if (command_0(LIB_NAMES, &library, &current_lib, &e_func, &convert_func) != OK) 
                    return ER_DLOPEN;
                break;
            case 1:
                command_1(e_func);
                break;
            case 2:
                command_2(convert_func);
                break;
            default:
                {
                   const char msg[] = "unknown command\n";
                   write(STDOUT_FILENO, msg, sizeof(msg)-1);
                }
                break;
        }

        write(STDOUT_FILENO, "> ", 2);
    }

    if (library) dlclose(library);
    write(STDOUT_FILENO, "\n", 1);
    return OK;
}
