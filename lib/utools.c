#include "utools.h"
#include <limits.h>
#include <stdlib.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>

char* replace_substring(const char* str, const char* old_substr, const char* new_substr) {
    char* result = (char*)malloc(strlen(str) + 1);
    strcpy(result, str);

    int old_substr_len = strlen(old_substr);

    if (strlen(str) <= old_substr_len) {
        free(result);
        return result;
    }

    char* str_result = (char*)malloc(strlen(old_substr) + 1);
    strncpy(str_result, str, old_substr_len);

    if (strcmp(old_substr, str_result)) {
        free(result);
        free(str_result);
        return result;
    }

    strncpy(str_result, str + old_substr_len, strlen(str) - old_substr_len);
    strcpy(result, "");

    strcat(result, new_substr);
    strcat(result, str_result);

    free(str_result);
    return result;
}
