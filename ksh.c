#include "lib/alias.h"
#include "lib/pipe.h"
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

static void ksh_loop(char* command, char** args);
static void resource_configuration(char** args);
static char* command_generator(const char* text, int state);
static char** my_completion(const char* text, int start, int end);
static void read_command(char* command);
static void parse_command(char* command, char** args);
static void change_directory(char** args);
static void execute_command(char** args);
static void run_file(char* file_path, char** args);

// TODO: runnig script file
void run_file(char* file_path, char** args) {
    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        return;
    }

    char line[MAX_COMMAND_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line)) {
            parse_command(line, args);
            if (args[0] != NULL) {
                execute_command(args);
            }
        }
    }

    fclose(file);
}

char* command_generator(const char* text, int state) {
    static int list_index, len;
    char* name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    while ((name = (char*)commands[list_index++])) {
        if (strncasecmp(name, text, len) == 0) {
            return strdup(name);
        }
    }

    return NULL;
}

char** my_completion(const char* text, int start, int end) {
    (void)start;
    (void)end;

    return rl_completion_matches(text, command_generator);
}

void read_command(char* command) {
    char dir[PATH_MAX];
    getcwd(dir, sizeof(dir));
    char* HOME_dir = getenv("HOME");
    char* prefix_dir = replace_substring(dir, HOME_dir, "~");

    printf("\n");
    printf("\033[1;37m%s\033[0m \033[1;36m%s\033[0m\n", getenv("LOGNAME"), prefix_dir);

    char* input = readline("\033[1;32mλ\033[0m ");
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
    char* HOME_dir = getenv("HOME");
    if (args[1] == NULL) {
        chdir(HOME_dir);
        return;
    }
    char* prefix_dir = replace_substring(args[1], "~", HOME_dir);
    if (chdir(prefix_dir)) {
        perror("chdir failed");
    }
    free(prefix_dir);
}

void execute_command(char** args) {
    char* alias_command = find_alias(args[0]);
    if (alias_command) {
        char command[MAX_COMMAND_LENGTH];
        strcpy(command, alias_command);
        parse_command(command, args);
    }

    if (!strcmp(args[0], "cd")) {
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

void resource_configuration(char** args) {
    char* HOME_dir = getenv("HOME");
    char* config_path = (char*)malloc(strlen(HOME_dir) + 8);
    strcpy(config_path, HOME_dir), strcat(config_path, "/.kshrc");

    run_file(config_path, args);
}

void ksh_loop(char* command, char** args) {
    rl_attempted_completion_function = my_completion;

    while (1) {
        read_command(command);

        if (!strcmp(command, "exit")) {
            break;
        }

        parse_command(command, args);
        if (args[0] == NULL) {
            continue;
        }

        int has_pipe = 0;
        for (int i = 0; args[i]; ++i) {
            if (strcmp(args[i], "|")) {
                has_pipe = 1;
                break;
            }
        }
        if (has_pipe) {
            run_pipe(args);
        } else {
            execute_command(args);
        }
    }
}

int main(int argc, char* argv[]) {
    char command[MAX_COMMAND_LENGTH];
    char* args[MAX_ARGS];

    if (argc == 1) {
        resource_configuration(args);

        ksh_loop(command, args);
    } else {
        run_file(argv[2], args);
    }

    return EXIT_SUCCESS;
}
