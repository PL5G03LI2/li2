#include <ctype.h>
#include <stdio.h>

#include "helpers/coords.h"
#include "types.h"

iVec2 add_vec2(iVec2 a, iVec2 b)
{
    return (iVec2){a.x + b.x, a.y + b.y};
}

bool assert_pos(Tab *tab, int x, int y)
{
    return (x >= 0 && x < tab->width && y >= 0 && y < tab->height);
}

int calc_index(Tab *tab, int x, int y)
{
    if (!assert_pos(tab, x, y))
        return -1;
    return x * tab->width + y;
}

iVec2 calc_pos(Tab *tab, int i)
{
    iVec2 pos = {-1, -1};
    if (!assert_index(tab, i))
        return pos;
    pos.x = i / tab->width;
    pos.y = i % tab->width;
    return pos;
}

bool assert_index(Tab *tab, int i)
{
    return (i >= 0 && i < tab->height * tab->width);
}

iVec2 read_coordinate(char *coord_tkn)
{
    iVec2 result;
    result.x = 0;
    result.y = 0;
    int i = 0;

    // couldn't find alpha char, return default.
    if (!isalpha(coord_tkn[i]))
        return result;

    // Read letters for x-coordinate
    while (isalpha(coord_tkn[i]))
    {
        result.x = result.x * 26 + (tolower(coord_tkn[i]) - 'a' + 1);
        i++;
    }

    // couldn't find a digit next, returns default.
    if (!isdigit(coord_tkn[i]))
    {
        result.x = 0;
        return result;
    }

    // Read numbers for y-coordinate
    while (isdigit(coord_tkn[i]))
    {
        result.y = result.y * 10 + (coord_tkn[i] - '0');
        i++;
    }

    // fix indexing
    result.x -= 1;
    result.y -= 1;

    return result;
}

char *write_coordinate(iVec2 coord, char *buffer)
{
    int pos = 0;
    int x = coord.x + 1;
    char letters[8] = {0};
    int letter_count = 0;

    while (x > 0)
    {
        int remainder = x % 26;
        if (remainder == 0)
        {
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
