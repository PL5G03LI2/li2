#include "helpers/coords.h"
#include "types.h"

iVec2 add_vec2(iVec2 a, iVec2 b)
{
    return (iVec2){a.x + b.x, a.y + b.y};
}

bool assert_pos(Tab *tab, int x, int y)
{
    return (x >= 0 && x < tab->height && y >= 0 && y < tab->width);
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
