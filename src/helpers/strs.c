#include <stdlib.h>
#include <string.h>
#include "helpers/strs.h"

int isLower(char c)
{
    return ('a' <= c && c <= 'z');
}

int isUpper(char c)
{
    return ('A' <= c && c <= 'Z');
}

char toUpper(char c)
{
    if (isLower(c))
        return c - 32;

    return c;
}

char toLower(char c)
{
    if (isUpper(c))
        return c + 32;

    return c;
}

char *trim_str(char *str)
{
    if (!str)
        return NULL;

    while (*str == ' ')
        str++;

    const char *end = str + strlen(str);
    while (end > str && *(end - 1) == ' ')
        end--;

    size_t len = end - str;
    char *out = malloc(len + 1);
    if (!out)
        return NULL;

    memcpy(out, str, len);
    out[len] = '\0';
    return out;
}
