#include "theme.h"
#include "util.h"
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

Theme *theme_start, *theme_end;
char *prefix_char;
int theme_len = 0;

// ========================================= get style

char *colors[] = { "black", "dark_red", "green", "yellow", "blue", "purple", "dark_green", "white" };

int get_color_idx(const char *color, int start_idx) {
        for (int i = 0; i < 8; ++i) {
                if (!strcmp(color, colors[i])) {
                        return i + start_idx;
                }
        }
        return 0;
}

// stdin a string like (foreground background){string}
// return a string like \033[1;fg;bgm%s\033[0m
char *get_style(const char *text) {
        char *style = (char *)malloc(24);
        strcpy(style, "\033[1");

        int idx;
        char color[4];

        // get foreground color
        int foreground_color_idx = strstr(text, ";") - text;
        char *fg = (char *)malloc(foreground_color_idx);
        strncpy(fg, text + 1, foreground_color_idx - 1);
        idx = get_color_idx(fg, 30);
        if (idx) {
                sprintf(color, ";%d", idx);
                strcat(style, color);
        }

        // get background color
        int background_color_idx = strstr(text, ")") - text;
        char *bg = (char *)malloc(background_color_idx - foreground_color_idx);
        strncpy(bg, text + foreground_color_idx + 1,
                background_color_idx - foreground_color_idx - 1);
        idx = get_color_idx(bg, 40);
        if (idx) {
                sprintf(color, ";%d", idx);
                strcat(style, color);
        }

        strcat(style, "m%s\033[0m ");

        free(fg);
        free(bg);
        return style;
}

// ========================================= get text
// TODO: git

char *return_str(char *str) {
        return str;
}

char *prefix() {
        return prefix_char;
}

char *hostname() {
        return getenv("LOGNAME");
}

char *directory() {
        char temp_dir[PATH_MAX];
        getcwd(temp_dir, sizeof(temp_dir));
        char *HOME_dir = getenv("HOME");
        char *dir = replace_substring(temp_dir, HOME_dir, "~", 1);

        return dir;
}

char *(*handle_theme_text(const char *text))() {
        if (!strcmp(text, "hostname")) {
                return &hostname;
        } else if (!strcmp(text, "directory")) {
                return &directory;
        } else if (!strcmp(text, "prefix")) {
                return &prefix;
        }

        return NULL;
}

char *get_text(const char *text) {
        char *temp = strstr(text, "{");
        int idx = temp - text;
        int len = strlen(text) - idx - 1;
        char *result = (char *)malloc(len + 1);
        strncpy(result, text + idx + 1, len - 1);

        return result;
}

// =========================================

void theme_init() {
        Theme *node = (Theme *)malloc(sizeof(Theme));
        node->style = "endl";
        theme_start = theme_end = node;
}

void theme_fini() {
        Theme *node = (Theme *)malloc(sizeof(Theme));
        node->style = "endl";
        theme_end->next = node;
        theme_end = node;
}

void theme_add_line(char **args) {
        theme_len++;

        if (theme_start == NULL) {
                theme_init();
        }

        for (int i = 1; args[i]; ++i) {
                Theme *node = (Theme *)malloc(sizeof(Theme));

                // get style
                char *style = get_style(args[i]);
                node->style = style;

                // get text
                char *result = get_text(args[i]);
                char *(*text)() = handle_theme_text(result);
                node->fn = text;
                free(result);

                theme_end->next = node;
                theme_end = node;
        }

        theme_fini();
}

void set_prefix_char(char *str) {
        prefix_char = (char *)malloc(strlen(str) + 1);
        strcpy(prefix_char, str);
}

int get_theme_len() {
        return theme_len;
}

Theme *get_theme_start() {
        return theme_start;
}
