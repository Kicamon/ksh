#include "lib/alias.h"
#include "lib/utools.h"
#include <limits.h>
#include <pwd.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGS 64

const char* commands[] = { "gd", "ls", "exit", "echo", "pwd", NULL };

static char* command_generator(const char* text, int state);
static char** my_completion(const char* text, int start, int end);
static void read_command(char* command);
static void parse_command(char* command, char** args);
static void change_directory(char** args);
static void execute_command(char** args);

char* command_generator(const char* text, int state) {
    static int list_index, len;
    char* name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    while ((name = (char*)commands[list_index++])) {
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }

    return NULL;
}

char** my_completion(const char* text, int start, int end) {
    char** matches = NULL;

    if (start == 0) {
        matches = rl_completion_matches(text, command_generator);
    }

    return matches;
}

void read_command(char* command) {
    char dir[PATH_MAX];
    getcwd(dir, sizeof(dir));
    char* HOME_dir = getenv("HOME");
    char* prefix_dir = replace_substring(dir, HOME_dir, "~");

    printf("\n");
    printf("KicamonIce %s\n", prefix_dir);

    char* input = readline("λ ");
    if (input && *input) {
        add_history(input);
        strncpy(command, input, MAX_COMMAND_LENGTH - 1);
        command[MAX_COMMAND_LENGTH - 1] = '\0';
        free(input);
    }

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
    char* alias_command = find_alias(args[0]);
    if (alias_command) {
        char command[MAX_COMMAND_LENGTH];
        strcpy(command, alias_command);
        parse_command(command, args);
    }

    if (!strcmp(args[0], "gd")) {
        change_directory(args);
    } else if (!strcmp(args[0], "alias")) {
        handle_alias_command(args);
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

    rl_attempted_completion_function = my_completion;

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
