#include "utools.h"
#include <limits.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MIN(A, B) ((A) < (B) ? (A) : (B))

int get_substring_num(const char* str, const char* substr, const int substr_len) {
    for (int i = 0, idx = 0; i < INT_MAX; ++i) {
        char* tmp_str = strstr(str + idx, substr);
        if (!tmp_str) {
            return i;
        }

        idx = tmp_str - str + substr_len;
    }

    return 0;
}

char* replace_substring(char* str, const char* old_substr, const char* new_substr, int sub_num) {
    int old_substr_len = strlen(old_substr);
    int new_substr_len = strlen(new_substr);
    int str_len = strlen(str);

    if (str_len < old_substr_len || !strstr(str, old_substr)) {
        return str;
    }

    sub_num = MIN(sub_num, get_substring_num(str,old_substr,old_substr_len));

    char* result;
    if (new_substr_len > old_substr_len) {
        int mem_need = str_len + (old_substr_len - new_substr_len) * sub_num;
        result = (char*)malloc(mem_need + 1);
    } else {
        result = (char*)malloc(str_len + 1);
    }
    strcpy(result, str);

    char* prefix = (char*)malloc(str_len + 1);
    char* suffix = (char*)malloc(str_len + 1);

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
