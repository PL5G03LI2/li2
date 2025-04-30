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

int carregar_jogo(Game *game, const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return 1;

    // Read dimensions
    int h, w;
    if (fscanf(f, "%d %d", &h, &w) != 2)
    {
        fclose(f);
        return 1;
    }

    free_game(game);
    init_game(game);

    Tab *tab = game->tabuleiro;

    // Initialize board
    tab->height = h;
    tab->width = w;
    free(tab->data);
    tab->data = calloc(h * w, sizeof(Piece));
    if (!tab->data)
    {
        fclose(f);
        return 1;
    }

    // Read board state
    for (int i = 0; i < h * w; i++)
    {
        Piece *p = &tab->data[i];
        if (fscanf(f, " %c", &p->c) != 1)
        {
            fclose(f);
            return 1;
        }
        p->marked = false;
        p->violated = false;
    }

    // Read "selected" marker
    char buffer[32];
    if (fscanf(f, "%s", buffer) != 1)
    {
        fclose(f);
        return 1;
    }

    // Read selected piece coords
    if (fscanf(f, "%d %d", &tab->sel_piece.x, &tab->sel_piece.y) != 2)
    {
        fclose(f);
        return 1;
    }

    // Read "marked" marker
    if (fscanf(f, "%s", buffer) != 1)
    {
        fclose(f);
        return 1;
    }

    // Read marked pieces until "violated" marker
    int x, y;
    while (fscanf(f, "%d %d", &x, &y) == 2)
    {
        int idx = calc_index(tab, x, y);
        if (idx >= 0)
            tab->data[idx].marked = true;
    }

    if (fscanf(f, "%s", buffer) != 1 || strcmp(buffer, "violated") != 0)
    {
        fclose(f);
        return 1;
    }

    // Read violated pieces until "history" marker
    while (fscanf(f, "%d %d", &x, &y) == 2)
    {
        int idx = calc_index(tab, x, y);
        if (idx >= 0)
            tab->data[idx].violated = true;
    }

    // Read "history" marker
    if (fscanf(f, "%s", buffer) != 1 || strcmp(buffer, "history") != 0)
    {
        fclose(f);
        return 1;
    }

    // Read command history - skip empty lines and verify format
    char token1[32], token2[32];
    char line[256];
    while (fgets(line, sizeof(line), f))
    {
        // Skip empty lines
        if (line[0] == '\n' || line[0] == '\0')
            continue;

        if (sscanf(line, "%s %s", token1, token2) == 2)
        {
            snprintf(game->cmd_str, 256, "%s %s", token1, token2);
            if (parse_command(game))
            {
                fclose(f);
                return 1;
            }
            game->history = push_history(game->history, game->cmd);
        }
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