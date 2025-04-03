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