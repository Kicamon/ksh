#include "utools.h"
#include <limits.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* replace_substring(const char* str, const char* old_substr, const char* new_substr) {
    char* result = (char*)malloc(strlen(str) + 1);
    strcpy(result, str);

    int old_substr_len = strlen(old_substr);

    if (strlen(str) < old_substr_len) {
        return result;
    }

    char* str_result = (char*)malloc(old_substr_len + 1);
    strncpy(str_result, str, old_substr_len);

    if (strncmp(old_substr, str_result, old_substr_len)) {
        free(str_result);
        return result;
    }

    strncpy(str_result, str + old_substr_len, strlen(str) - old_substr_len);
    str_result[strlen(str) - old_substr_len] = '\0';
    strcpy(result, "");

    strcat(result, new_substr);
    strcat(result, str_result);

    free(str_result);
    return result;
}
