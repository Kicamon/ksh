#include "lib/utools.h"
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGS 64
#define MAX(A, B) ((A) > (B) ? (A) : (B))

void read_command(char* command) {
    char dir[PATH_MAX];
    getcwd(dir, sizeof(dir));
    char* HOME_dir = getenv("HOME");
    char* prefix_dir = replace_substring(dir, HOME_dir, "~");

    printf("\n");
    printf("KicamonIce %s\n", prefix_dir);
    printf("λ ");
    fgets(command, MAX_COMMAND_LENGTH, stdin);
    command[strcspn(command, "\n")] = 0;

    free(prefix_dir);
}

void parse_command(char* command, char** args) {
    char* token;
    int i = 0;
    token = strtok(command, " ");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
}

void change_directory(char** args) {
    if (args[1] == NULL) {
        char* HOME_dir = getenv("HOME");
        chdir(HOME_dir);
        return;
    }
    if (chdir(args[1])) {
        perror("chdir failed");
    }
}

void execute_command(char** args) {
    if (!strcmp(args[0], "cd")) {
        change_directory(args);
    } else {
        pid_t pid = fork();
        if (pid == 0) {
            if (execvp(args[0], args) == -1) {
                perror("execvp failed");
            }
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            int status;
            waitpid(pid, &status, 0);
        } else {
            perror("fork failed");
        }
    }
}

int main() {
    char command[MAX_COMMAND_LENGTH];
    char* args[MAX_ARGS];

    while (1) {
        read_command(command);

        if (strcmp(command, "exit") == 0) {
            break;
        }

        parse_command(command, args);
        if (args[0] != NULL) {
            execute_command(args);
        }
    }

    return 0;
}
