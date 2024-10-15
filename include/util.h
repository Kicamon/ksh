#ifndef _UTIL_H
#define _UTIL_H

#define MIN(A, B)               ((A) < (B) ? (A) : (B))
#define MAX(A, B)               ((A) > (B) ? (A) : (B))
#define BETWEEN(X, A, B)        ((A) <= (X) && (X) <= (B))

char *replace_substring(char *str, const char *old_substr, const char *new_substr, int sub_num);
int pscanf(const char *path, const char *fmt, ...);
int cscanf(const char *cmd, const char *fmt, ...);

#endif // !_UTIL_H
