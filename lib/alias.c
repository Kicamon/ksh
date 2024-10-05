#include "alias.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Alias *alias_start = NULL, *alias_end = NULL;

void list_alias() {
    if (alias_start == NULL) {
        return;
    }
    for (Alias* c = alias_start; c; c = c->next) {
        printf("%s=%s\n", c->alias, c->command);
    }
}

char* find_alias(const char* alias) {
    if (alias_start == NULL) {
        return NULL;
    }
    for (Alias* c = alias_start; c; c = c->next) {
        if (!strcmp(c->alias, alias)) {
            return c->command;
        }
    }
    return NULL;
}

void add_alias(const char* alias, const char* command) {
    char* check = find_alias(alias);
    if (check != NULL && !strcmp(command, check)) {
        return;
    }

    Alias* node = (Alias*)malloc(sizeof(Alias));
    node->alias = strdup(alias), node->command = strdup(command);

    if (alias_start == NULL) {
        alias_start = alias_end = node;
    } else {
        alias_end->next = node;
        alias_end = node;
    }
}

void handle_alias_command(char** args) {
    if (args[1] == NULL) {
        list_alias();
        return;
    }

    char* alias = strtok(args[1], "=");
    char* command = strtok(NULL, "=");
    if (alias && command) {
        add_alias(alias, command);
    } else {
        printf("Invalid alias format. Use: alias name='command'\n");
    }
}
