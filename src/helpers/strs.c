#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "helpers/strs.h"

char *trim_str(char *str) {
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
