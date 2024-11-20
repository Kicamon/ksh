#include "../include/util.h"
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int get_substring_num(const char *str, const char *substr, const int substr_len);

int get_substring_num(const char *str, const char *substr, const int substr_len) {
        for (int i = 0, idx = 0; i < INT_MAX; ++i) {
                char *tmp_str = strstr(str + idx, substr);
                if (!tmp_str) {
                        return i;
                }

                idx = tmp_str - str + substr_len;
        }

        return 0;
}

// Quickly dynamic substitute string
void replace_substring(char **str, const char *old_substr, const char *new_substr, int sub_num) {
        size_t str_len = strlen(*str);
        size_t old_substr_len = strlen(old_substr);
        size_t new_substr_len = strlen(new_substr);

        if (str_len < old_substr_len || !strstr(*str, old_substr)) {
                return;
        }

        int total_subnum = get_substring_num(*str, old_substr, old_substr_len);
        sub_num = (sub_num <= 0 || sub_num > total_subnum) ? total_subnum : sub_num;

        char *prefix = (char *)malloc(str_len + 1);
        char *suffix = (char *)malloc(str_len + 1);
        int new_str_len = str_len + (new_substr_len - old_substr_len) * sub_num;
        char *new_str = (char *)malloc(new_str_len + 1);
        strcpy(new_str, *str);

        for (int i = 0; i < sub_num; ++i) {
                int prefix_len = strstr(new_str, old_substr) - new_str;
                int suffix_start_postion = prefix_len + old_substr_len;
                int suffix_len = new_str_len - suffix_start_postion;

                strncpy(prefix, new_str, prefix_len);
                prefix[prefix_len] = '\0';
                strncpy(suffix, new_str + suffix_start_postion, suffix_len);
                suffix[suffix_len] = '\0';

                strcpy(new_str, prefix);
                strcat(new_str, new_substr);
                strcat(new_str, suffix);
        }

        free(*str);
        free(prefix);
        free(suffix);
        *str = new_str;
}

int pscanf(const char *path, const char *fmt, ...) {
        FILE *fp = NULL;
        va_list ap;
        int n;

        if (!(fp = fopen(path, "r"))) {
                return -1;
        }
        va_start(ap, fmt);
        n = vfscanf(fp, fmt, ap);
        va_end(ap);
        fclose(fp);

        return (n == EOF) ? -1 : n;
}

int cscanf(const char *cmd, const char *fmt, ...) {
        FILE *cp = NULL;
        va_list ap;
        int n;

        if (!(cp = popen(cmd, "r"))) {
                return -1;
        }
        va_start(ap, fmt);
        n = vfscanf(cp, fmt, ap);
        va_end(ap);
        pclose(cp);

        return (n == EOF) ? -1 : n;
}
