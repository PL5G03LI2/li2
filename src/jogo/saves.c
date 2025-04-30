#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helpers/coords.h"
#include "helpers/history.h"
#include "helpers/memory.h"

#include "jogo/comandos.h"
#include "jogo/saves.h"
#include "jogo/tabuleiro.h"
#include "types.h"

int read_board(FILE *f, Tab *tab, int h, int w)
{
    tab->height = h;
    tab->width = w;
    free(tab->data);
    tab->data = calloc(h * w, sizeof(Piece));
    if (!tab->data)
        return 1;

    for (int i = 0; i < h * w; i++)
    {
        Piece *p = &tab->data[i];
        if (fscanf(f, " %c", &p->c) != 1)
            return 1;
        p->marked = false;
        p->violated = false;
    }
    return 0;
}

int read_selected(FILE *f, Tab *tab)
{
    char buffer[32];
    if (fscanf(f, "%s", buffer) != 1)
        return 1;

    if (fscanf(f, "%d %d", &tab->sel_piece.x, &tab->sel_piece.y) != 2)
        return 1;

    return 0;
}

int read_marked(FILE *f, Tab *tab)
{
    char buffer[32];
    if (fscanf(f, "%s", buffer) != 1)
        return 1;

    int x, y;
    while (fscanf(f, "%d %d", &x, &y) == 2)
    {
        int idx = calc_index(tab, x, y);
        if (idx >= 0)
            tab->data[idx].marked = true;
    }

    return 0;
}

int read_violated(FILE *f, Tab *tab)
{
    char buffer[32];
    if (fscanf(f, "%s", buffer) != 1 || strcmp(buffer, "violated") != 0)
        return 1;

    int x, y;
    while (fscanf(f, "%d %d", &x, &y) == 2)
    {
        int idx = calc_index(tab, x, y);
        if (idx >= 0)
            tab->data[idx].violated = true;
    }

    return 0;
}

int read_history(FILE *f, Game *game)
{
    char buffer[32];
    if (fscanf(f, "%s", buffer) != 1 || strcmp(buffer, "history") != 0)
        return 1;

    char token1[32], token2[32];
    char line[256];
    while (fgets(line, sizeof(line), f))
    {
        if (line[0] == '\n' || line[0] == '\0')
            continue;

        if (sscanf(line, "%s %s", token1, token2) == 2)
        {
            snprintf(game->cmd_str, 256, "%s %s", token1, token2);
            if (parse_command(game))
                return 1;

            game->history = push_history(game->history, game->cmd);
        }
    }

    return 0;
}

int carregar_jogo(Game *game, const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return 1;

    int h, w;
    if (fscanf(f, "%d %d", &h, &w) != 2)
    {
        fclose(f);
        return 1;
    }

    free_game(game);
    init_game(game);

    Tab *tab = game->tabuleiro;

    if (read_board(f, tab, h, w))
    {
        fclose(f);
        return 1;
    }

    if (read_selected(f, tab))
    {
        fclose(f);
        return 1;
    }

    if (read_marked(f, tab))
    {
        fclose(f);
        return 1;
    }

    if (read_violated(f, tab))
    {
        fclose(f);
        return 1;
    }

    if (read_history(f, game))
    {
        fclose(f);
        return 1;
    }

    fclose(f);
    return 0;
}

int salvar_jogo(Game *game, const char *filename)
{
    FILE *f = fopen(filename, "w");
    if (!f)
        return 1;

    Tab *tab = game->tabuleiro;

    fprintf(f, "%d %d\n", tab->height, tab->width);

    int total_size = tab->height * tab->width;

    for (int i = 0; i < total_size; i++)
    {
        Piece p = tab->data[i];
        fprintf(f, "%c", p.c);
        if ((i + 1) % tab->width == 0)
        {
            fprintf(f, "\n");
        }
    }
    fprintf(f, "\n");

    fprintf(f, "selected\n");
    fprintf(f, "%d %d\n", tab->sel_piece.x, tab->sel_piece.y);

    fprintf(f, "marked\n");
    for (int i = 0; i < total_size; i++)
    {
        Piece p = tab->data[i];

        iVec2 pos = calc_pos(tab, i);
        if (p.marked)
            fprintf(f, "%d %d\n", pos.x, pos.y);
    }

    fprintf(f, "violated\n");
    for (int i = 0; i < total_size; i++)
    {
        Piece p = tab->data[i];

        iVec2 pos = calc_pos(tab, i);
        if (p.violated)
            fprintf(f, "%d %d\n", pos.x, pos.y);
    }

    fprintf(f, "history\n");
    int i = 0;
    ParsedCommand *current = get_history_element(game->history, i++);
    while (current != NULL)
    {
        for (int i = 0; i < 2; i++)
            fprintf(f, "%s ", current->tokens[i]);
        fprintf(f, "\n");
        current = get_history_element(game->history, i++);
    }

    fclose(f);
    return 0;
}