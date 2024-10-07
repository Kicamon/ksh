typedef struct Alias Alias;

struct Alias {
        char *alias;
        char *command;
        Alias *next;
};

void list_alias();
char *find_alias(const char *alias);
void add_alias(const char *alias, const char *command);
void handle_alias_command(char **args);
