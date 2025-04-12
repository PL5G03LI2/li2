#include <stdlib.h>
#include <string.h>
#include "strs.h"

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

    int len = strlen(str);
    int start = 0;
    int end = len - 1;

    while (start < len && str[start] == ' ')
        start++;

    while (end > start && str[end] == ' ')
        end--;

    int new_len = end - start + 1;

    if (new_len <= 0)
    {
        free(str);
        return strdup("");
    }

    char *new_str = (char *)malloc(new_len + 1);
    if (!new_str)
    {
        free(str);
        return NULL;
    }

    memcpy(new_str, str + start, new_len);
    new_str[new_len] = '\0';

    free(str);
    return new_str;
}
