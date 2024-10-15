#ifndef _KSHFUNCS_H
#define _KSHFUNCS_H

typedef struct Alias Alias;

struct Alias {
        char *alias;
        char *command;
        Alias *next;
};

char *find_alias(const char *alias);
void handle_alias_command(char **args);
void run_pipe(char **args);

#endif // !_KSHFUNCS_H
