#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/libs.h"

#define BUFFER_SIZE 4096

void command_1() {
    char* arg1 = strtok(NULL, " \t\n");
    
    int len = 0;
    char buffer[BUFFER_SIZE];

    if (arg1) {
        int x = atoi(arg1);
        float res = e(x);
        len = snprintf(buffer, BUFFER_SIZE, "e(%d) result: %.6f\n", x, res);
        write(STDOUT_FILENO, buffer, len);
    } else {
        const char msg[] = "error: missing argument for command 1\n";
        write(STDERR_FILENO, msg, sizeof(msg)-1);
    }
}

void command_2() {
    char* arg1 = strtok(NULL, " \t\n");
    
    int len = 0;
    char buffer[BUFFER_SIZE];

    if (arg1) {
        int x = atoi(arg1);
        char* res = convert(x);
        
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
    {
        const char* message = "static prog\ncommands: 1 - e_func; 2 - convert; ctrl d to exit\n> ";
        write(STDOUT_FILENO, message, strlen(message));
    }

    int bytes_read = 0;
    char buffer[BUFFER_SIZE];

    while((bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = 0;

        char* token = strtok(buffer, " \t\n");
        if (!token) {
            write(STDOUT_FILENO, "> ", 2);
            continue;
        }

        int cmd = atoi(token);
        switch (cmd) {
            case 1:
                command_1();
                break;
            case 2:
                command_2();
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

    write(STDOUT_FILENO, "\n", 1);
    return 0;
}
