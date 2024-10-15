#include "../include/util.h"
#include "../include/kshfuncs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static void list_alias();
static void add_alias(const char *alias, const char *command);

static Alias *alias_start = NULL, *alias_end = NULL;

// alias
void list_alias() {
        if (alias_start == NULL) {
                return;
        }
        for (Alias *c = alias_start; c; c = c->next) {
                printf("%s=%s\n", c->alias, c->command);
        }
}

char *find_alias(const char *alias) {
        if (alias_start == NULL) {
                return NULL;
        }
        for (Alias *c = alias_start; c; c = c->next) {
                if (!strcmp(c->alias, alias)) {
                        return c->command;
                }
        }
        return NULL;
}

void add_alias(const char *alias, const char *command) {
        char *check = find_alias(alias);
        if (check != NULL && !strcmp(command, check)) {
                return;
        }

        Alias *node = (Alias *)malloc(sizeof(Alias));
        node->alias = strdup(alias), node->command = strdup(command);

        if (alias_start == NULL) {
                alias_start = alias_end = node;
        } else {
                alias_end->next = node;
                alias_end = node;
        }
}

void handle_alias_command(char **args) {
        if (args[1] == NULL) {
                list_alias();
                return;
        }

        char *arg = (char *)malloc(sizeof(args));
        strcpy(arg, args[1]);
        for (int i = 2; args[i]; ++i) {
                strcat(arg, " ");
                strcat(arg, args[i]);
        }

        char *alias = strtok(arg, "=");
        char *command = strtok(NULL, "=");
        if (alias && command) {
                add_alias(alias, command);
        } else {
                printf("Invalid alias format. Use: alias name='command'\n");
        }
}

char ***handle_pipe(char **args, int *commands_num) {
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

        char ***commands = (char ***)malloc(len * sizeof(char **));
        for (int i = 0; i < len; ++i) {
                commands[i] = (char **)malloc(command_max_len * sizeof(char *));
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

// pipe
void run_pipe(char **args) {
        int commands_num = 0;
        char ***commands = handle_pipe(args, &commands_num);
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
