#include "pipe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX(A, B) ((A) > (B) ? (A) : (B))

char*** handle_pipe(char** args, int* commands_num) {
    int command_max_len = 0, command_len = 0, len = 1;
    for (int i = 0; args[i]; ++i) {
        if (!strcmp(args[i], "|")) {
            command_max_len = MAX(command_max_len, command_len);
            command_len = 0;
            len++;
            continue;
        }
        command_len++;
    }
    command_max_len = MAX(command_max_len, command_len) + 1;
    *commands_num = len;

    char*** commands = (char***)malloc(len * sizeof(char**));
    for (int i = 0; i < len; ++i) {
        commands[i] = (char**)malloc(command_max_len * sizeof(char*));
    }

    int row = 0, col = 0;
    for (int i = 0; args[i]; ++i) {
        if (!strcmp(args[i], "|")) {
            commands[row][col] = NULL;
            row++;
            col = 0;
            continue;
        }
        commands[row][col++] = args[i];
    }
    commands[row][col] = NULL;

    return commands;
}

void run_pipe(char** args) {
    int commands_num = 0;
    char*** commands = handle_pipe(args, &commands_num);
    int pipefds[2 * (commands_num - 1)];

    for (int i = 0; i < commands_num - 1; i++) {
        if (pipe(pipefds + i * 2) == -1) {
            perror("pipe failed");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < commands_num; ++i) {
        pid_t pid = fork();
        if (pid > 0) {
            continue;
        }

        if (i) {
            dup2(pipefds[(i - 1) * 2], STDIN_FILENO);
        }

        if (i < commands_num - 1) {
            dup2(pipefds[i * 2 + 1], STDOUT_FILENO);
        }

        for (int j = 0; j < 2 * (commands_num - 1); j++) {
            close(pipefds[j]);
        }

        execvp(commands[i][0], commands[i]);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 2 * (commands_num - 1); i++) {
        close(pipefds[i]);
    }

    for (int i = 0; i < commands_num; i++) {
        wait(NULL);
    }

    for (int i = 0; i < commands_num; ++i) {
        free(commands[i]);
    }
    free(commands);
}
