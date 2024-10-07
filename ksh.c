#include "lib/alias.h"
#include "lib/pipe.h"
#include "lib/util.h"
#include "lib/theme.h"
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

const char *commands[] = { "cd", "ls", "exit", "echo", "pwd", NULL };
int running = 1;

static void ksh_loop(char *command, char **args);
static void resource_configuration(char **args);
static char *command_generator(const char *text, int state);
static char **ksh_completion(const char *text, int start, int end);
static void read_command(char *command);
static void parse_command(char *command, char **args);
static void change_directory(char **args);
static void execute_command(char **args);
static void run_file(char *file_path, char **args);

// TODO: improve the execution of the file
// execute file
void run_file(char *file_path, char **args) {
        FILE *file = fopen(file_path, "r");
        if (file == NULL) {
                return;
        }

        // read file by line and running
        char line[MAX_COMMAND_LENGTH];
        while (fgets(line, sizeof(line), file) != NULL) {
                line[strcspn(line, "\n")] = 0;
                if (!strlen(line)) {
                        continue;
                }
                parse_command(line, args);
                if (args[0] != NULL) {
                        execute_command(args);
                }
        }

        fclose(file);
}

char *command_generator(const char *text, int state) {
        static int list_index, len;
        char *name;

        if (!state) {
                list_index = 0;
                len = strlen(text);
        }

        while ((name = (char *)commands[list_index++])) {
                if (strncasecmp(name, text, len) == 0) {
                        return strdup(name);
                }
        }

        return NULL;
}

char **ksh_completion(const char *text, int start, int end) {
        (void)start;
        (void)end;

        return rl_completion_matches(text, command_generator);
}

void read_command(char *command) {
        Theme *theme_start = get_theme_start();
        char theme_line[MAX_COMMAND_LENGTH] = "", tmp[MAX_COMMAND_LENGTH];
        int idx = 0, theme_len = get_theme_len();
        for (Theme *p = theme_start; p; p = p->next) {
                if (!strcmp(p->style, "endl")) {
                        if (idx == theme_len) {
                                break;
                        }
                        idx++;
                        printf("%s\n", theme_line);
                        strcpy(theme_line, "");
                        continue;
                }
                sprintf(tmp, p->style, p->fn());
                strcat(theme_line, tmp);
        }

        char *input = readline(theme_line);
        if (input && *input) {
                add_history(input);
                strncpy(command, input, MAX_COMMAND_LENGTH - 1);
                command[MAX_COMMAND_LENGTH - 1] = '\0';
                free(input);
        }
}

// convert the command into arguments for execvp
void parse_command(char *command, char **args) {
        char *token;
        int i = 0;
        token = strtok(command, " ");
        while (token != NULL) {
                args[i++] = token;
                token = strtok(NULL, " ");
        }
        args[i] = NULL;
}

void change_directory(char **args) {
        char *HOME_dir = getenv("HOME");
        if (args[1] == NULL) {
                chdir(HOME_dir);
                return;
        }
        char *dir = replace_substring(args[1], "~", HOME_dir, 1);
        if (chdir(dir)) {
                perror("chdir failed");
        }
}

void execute_command(char **args) {
        // check and convert alias command
        char *alias_command = find_alias(args[0]);
        if (alias_command) {
                char command[MAX_COMMAND_LENGTH];
                strcpy(command, alias_command);
                parse_command(command, args);
        }
        if (!strcmp(args[0], "exit")) {
                running = 0;
        } else if (!strcmp(args[0], "THEME_PREFIX_CHAR")) {
                set_prefix_char(args[1]);
        } else if (!strcmp(args[0], "THEME")) {
                theme_add_line(args);
        } else if (!strcmp(args[0], "cd")) {
                change_directory(args);
        } else if (!strcmp(args[0], "alias")) {
                handle_alias_command(args);
        } else {
                // run the command in the child process
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

// read user's configuration file and init shell
void resource_configuration(char **args) {
        char *HOME_dir = getenv("HOME");
        char *config_path = (char *)malloc(strlen(HOME_dir) + 8);
        strcpy(config_path, HOME_dir), strcat(config_path, "/.kshrc");

        run_file(config_path, args);
}

void ksh_loop(char *command, char **args) {
        rl_attempted_completion_function = ksh_completion;
        int has_pipe = 0;

        while (running) {
                read_command(command);

                parse_command(command, args);
                if (args[0] == NULL) {
                        continue;
                }

                // check whether command has pipe
                for (int i = 0; args[i]; ++i) {
                        if (!strcmp(args[i], "|")) {
                                has_pipe = 1;
                                break;
                        }
                }
                if (has_pipe) {
                        has_pipe = 0;
                        run_pipe(args);
                } else {
                        execute_command(args);
                }
        }
}

int main(int argc, char *argv[]) {
        char command[MAX_COMMAND_LENGTH];
        char *args[MAX_ARGS];

        if (argc == 1) {
                resource_configuration(args);

                ksh_loop(command, args);
        } else {
                run_file(argv[2], args);
        }

        return EXIT_SUCCESS;
}
