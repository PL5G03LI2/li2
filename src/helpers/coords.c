#include <ctype.h>
#include <stdio.h>

#include "helpers/coords.h"
#include "types.h"

iVec2 read_coordinate(char *coord_tkn) {
  iVec2 result;
  result.x = 0;
  result.y = 0;
  int i = 0;

  // couldn't find alpha char, return default.
  if (!isalpha(coord_tkn[i]))
    return result;

  // Read letters for x-coordinate
  while (isalpha(coord_tkn[i])) {
    result.x = result.x * 26 + (tolower(coord_tkn[i]) - 'a' + 1);
    i++;
  }

  // couldn't find a digit next, returns default.
  if (!isdigit(coord_tkn[i])) {
    result.x = 0;
    return result;
  }

  // Read numbers for y-coordinate
  while (isdigit(coord_tkn[i])) {
    result.y = result.y * 10 + (coord_tkn[i] - '0');
    i++;
  }

  // fix indexing
  result.x -= 1;
  result.y -= 1;

  return result;
}

char *write_coordinate(iVec2 coord, char *buffer) {
  int pos = 0;
  int x = coord.x + 1;
  char letters[8] = {0};
  int letter_count = 0;

  while (x > 0) {
    int remainder = x % 26;
    if (remainder == 0) {
      remainder = 26;
      x -= 1;
    }
    letters[letter_count++] = 'a' + remainder - 1;
    x = x / 26;
  }

  for (int i = letter_count - 1; i >= 0; i--)
    buffer[pos++] = letters[i];

  // Write y coordinate
  sprintf(buffer + pos, "%d", coord.y + 1);

  return buffer;
}
