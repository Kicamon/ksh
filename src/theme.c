#include "../include/theme.h"
#include "../include/util.h"
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

static int get_color_idx(const char *color, int start_idx);
static char *get_style(const char *text);
static char *git_info();
static char *prefix();
static char *hostname();
static char *directory();
static char *(*handle_theme_text(const char *text))();
static char *get_text(const char *text);
static void theme_init();
static void theme_fini();

char *colors[] = { "black", "dark_red", "green", "yellow", "blue", "purple", "dark_green", "white" };
Theme *theme_start, *theme_end;
char *prefix_char;
int theme_len = 0;

// ===================== get style =====================

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

// ===================== get text =====================

char *git_info() {
        static char git_infos[256];

        char branch[20];
        int add = 0, del = 0, mdy = 0;

        // check is git repositore
        if (system("git rev-parse --is-inside-work-tree &>/dev/null")) {
                return "";
        }

        // get branch
        if (cscanf("git rev-parse --abbrev-ref HEAD", "%s", branch) != 1) {
                return "";
        }

        // get more info
        FILE *cp = NULL;
        cp = popen("git status --short", "r");
        if (!cp) {
                return "";
        }
        while (fgets(git_infos, sizeof(git_infos) - 1, cp)) {
                char temp[3];
                sscanf(git_infos, "%s", temp);
                add += temp[0] == '?';
                del += temp[0] == 'D';
                mdy += temp[0] == 'M';
        }
        pclose(cp);

        sprintf(git_infos, "<%s> +%d ~%d -%d", branch, add, mdy, del);

        return git_infos;
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
        } else if (!strcmp(text, "git_info")) {
                return &git_info;
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
        if (prefix_char == NULL) {
                prefix_char = (char *)malloc(strlen(str) + 1);
        }
        strcpy(prefix_char, str);
}

int get_theme_len() {
        return theme_len;
}

Theme *get_theme_start() {
        return theme_start;
}
