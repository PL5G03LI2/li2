#include <stdlib.h>
#include <ncurses.h>

#include "helpers/history.h"
#include "helpers/memory.h"
#include "jogo/tabuleiro.h"

#include "types.h"

Tab *initialize_tabuleiro(void)
{
    Tab *tab = malloc(sizeof(Tab));
    if (!tab)
        return NULL;
    tab->height = 0;
    tab->width = 0;
    tab->sel_piece.x = 0;
    tab->sel_piece.y = 0;
    tab->data = NULL;
    return tab;
}

int init_game(Game *game)
{
    game->tabuleiro = initialize_tabuleiro();
    if (game->tabuleiro == NULL)
        return 1;

    game->cmd_str = (char *)calloc(256, sizeof(char));
    if (game->cmd_str == NULL)
    {
        free_game(game);
        return 1;
    }

    game->cmd = (ParsedCommand *)malloc(sizeof(ParsedCommand));
    if (game->cmd == NULL)
    {
        free_game(game);
        return 1;
    }

    game->cmd->type = CMD_INVALID;
    game->cmd->track = false;

    game->cmd->tokens = (char **)calloc(2, sizeof(char *));
    if (game->cmd->tokens == NULL)
    {
        free_game(game);
        return 1;
    }

    for (int i = 0; i < 2; i++)
    {
        game->cmd->tokens[i] = (char *)calloc(32, sizeof(char));
        if (game->cmd->tokens[i] == NULL)
        {
            free_game(game);
            return 1;
        }
    }

    game->history = NULL;

    getmaxyx(stdscr, game->win_d.y, game->win_d.x);

    game->save_to = NULL;

    game->game_ui.main_win.win = newwin(0, 0, 0, 0);
    game->game_ui.main_win.pos = (iVec2){0, 0};
    game->game_ui.main_win.size = (iVec2){0, 0};

    game->game_ui.help_win.win = newwin(0, 0, 0, 0);
    game->game_ui.help_win.pos = (iVec2){0, 0};
    game->game_ui.help_win.size = (iVec2){0, 0};

    game->game_ui.cmd_win.win = newwin(0, 0, 0, 0);
    game->game_ui.cmd_win.pos = (iVec2){0, 0};
    game->game_ui.cmd_win.size = (iVec2){0, 0};

    game->game_ui.offsets = (iVec2){0, 0};

    return 0;
}

void free_command(ParsedCommand **cmd)
{
    if (*cmd != NULL && (*cmd)->tokens)
    {
        for (int i = 0; i < 2; i++)
            free((*cmd)->tokens[i]);

        free((*cmd)->tokens);
        free(*cmd);
        *cmd = NULL;
    }
}

void free_tabuleiro(Tab **tab)
{
    if (*tab)
    {
        if ((*tab)->data)
            free((*tab)->data);

        (*tab)->data = NULL;

        free(*tab);
    }

    *tab = NULL;
}

void destroy_history(TabHistory **head)
{
    TabHistory *current = *head;
    while (current != NULL)
    {
        TabHistory *next = current->next;
        if (current->cmd)
        {
            if (current->cmd->tokens)
            {
                for (int i = 0; i < 2; i++)
                    free(current->cmd->tokens[i]);
                free(current->cmd->tokens);
            }
            free(current->cmd);
        }
        free(current);
        current = next;
    }
    *head = NULL;
}

void free_game(Game *game)
{
    free_tabuleiro(&game->tabuleiro);

    if (game->cmd_str)
    {
        free(game->cmd_str);
        game->cmd_str = NULL;
    }

    free_command(&(game->cmd));

    destroy_history(&(game->history));

    free(game->save_to);

    delwin(game->game_ui.main_win.win);
    delwin(game->game_ui.help_win.win);
    delwin(game->game_ui.cmd_win.win);
}
