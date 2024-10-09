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

char *replace_substring(char *str, const char *old_substr, const char *new_substr, int sub_num) {
        static char result[PATH_MAX];
        int old_substr_len = strlen(old_substr);
        int new_substr_len = strlen(new_substr);
        int str_len = strlen(str);

        if (str_len < old_substr_len || !strstr(str, old_substr)) {
                return str;
        }

        sub_num = MIN(sub_num, get_substring_num(str, old_substr, old_substr_len));

        strcpy(result, str);

        char *prefix = (char *)malloc(str_len + 1);
        char *suffix = (char *)malloc(str_len + 1);

        for (int i = 0; i < sub_num; ++i) {
                int prefix_len = strstr(result, old_substr) - result;
                int suffix_start_idx = prefix_len + old_substr_len;
                int suffix_len = str_len - suffix_start_idx;

                strncpy(prefix, result, prefix_len);
                prefix[prefix_len] = '\0';
                strncpy(suffix, result + suffix_start_idx, suffix_len);
                suffix[suffix_len] = '\0';

                strcpy(result, prefix);
                strcat(result, new_substr);
                strcat(result, suffix);
        }

        free(prefix);
        free(suffix);

        return result;
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
