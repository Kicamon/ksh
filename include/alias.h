typedef struct Alias Alias;

struct Alias {
        char *alias;
        char *command;
        Alias *next;
};

char *find_alias(const char *alias);
void handle_alias_command(char **args);
