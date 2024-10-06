#include "utools.h"
#include <limits.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void replace_substring(char* str, const char* old_substr, const char* new_substr, int sub_num) {
    int old_substr_len = strlen(old_substr);
    int str_len = strlen(str);

    if (strlen(str) < old_substr_len) {
        return;
    }

    if (!strstr(str, old_substr)) {
        return;
    }

    for (int i = 0; i < sub_num; ++i) {
        int prefix_len = strstr(str, old_substr) - str;
        int tmp = prefix_len + old_substr_len;
        int suffix_len = str_len - tmp;

        char* prefix = (char*)malloc(prefix_len + 1);
        char* suffix = (char*)malloc(suffix_len + 1);

        strncpy(prefix, str, prefix_len);
        strncpy(suffix, str + tmp, suffix_len);
        prefix[prefix_len] = '\0';
        suffix[suffix_len] = '\0';

        strcpy(str, prefix);
        strcat(str, new_substr);
        strcat(str, suffix);

        free(prefix);
        free(suffix);
    }
}
