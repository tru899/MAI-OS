#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    char progpath[1024];
    {
        const char msg[] = "input file: ";
        write(STDOUT_FILENO, msg, sizeof(msg) - 1);

        ssize_t n = read(STDIN_FILENO, progpath, sizeof(progpath) - 1);
        if (n <= 0) {
            const char msg[] = "error: can't read the file\n";
            write(STDERR_FILENO, msg, sizeof(msg) - 1);
            exit(EXIT_FAILURE);
        }
        progpath[n - 1] = '\0';
    }

    //open pipes
    int client_to_server[2];
    if (pipe(client_to_server) == -1) {
        const char msg[] = "error: can't make the pipe\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        exit(EXIT_FAILURE);
    }

    int server_to_client[2];
    if (pipe(server_to_client) == -1) {
        const char msg[] = "error: can't make the pipe\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        exit(EXIT_FAILURE);
    }

    //make process
    pid_t pid = fork();

    switch(pid) {
        case -1: {
            const char msg[] = "error: can't make a new process\n";
            write(STDERR_FILENO, msg, sizeof(msg) - 1);
            exit(EXIT_FAILURE);
        } break;
        case 0: {
            //child process
            close(client_to_server[1]);
            close(server_to_client[0]);

            dup2(client_to_server[0], STDIN_FILENO);
            close(client_to_server[0]);

            dup2(server_to_client[1], STDOUT_FILENO);
            close(server_to_client[1]);

            execl("./server", "server", NULL);
            const char msg[] = "error: can't exec\n";
            write(STDERR_FILENO, msg, sizeof(msg) - 1);
            exit(EXIT_FAILURE);
        } break;
        default: {
            //parent process
            close(client_to_server[0]);
            close(server_to_client[1]); 

            int file = open(progpath, O_RDONLY);
            if (file == -1) {
                const char msg[] = "error: can't open file\n";
                write(STDERR_FILENO, msg, sizeof(msg) - 1);
                exit(EXIT_FAILURE);
            }

            char buf[4096];
            ssize_t n;
            while ((n = read(file, buf, sizeof(buf)))) {
                write(client_to_server[1], buf, n);
            }
            close(file);
            close(client_to_server[1]);

            //request from child
            while ((n = read(server_to_client[0], buf, sizeof(buf)))) {
                write(STDOUT_FILENO, buf, n);
            }
            close(server_to_client[0]);

            wait(NULL);
        } break;
    }

    return 0;
}
