#ifndef _THEME_H
#define _THEME_H

typedef struct Theme Theme;
typedef struct Theme_text Theme_text;
typedef char *(*text)();

struct Theme {
        char *style;
        text fn;
        Theme *next;
};

void theme_add_line(char **args);
void set_prefix_char(char *str);
int get_theme_len();
Theme *get_theme_start();

#endif // !_THEME_H
