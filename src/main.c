#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "helpers/history.h"
#include "helpers/memory.h"
#include "helpers/strs.h"
#include "jogo/comandos.h"
#include "jogo/tabuleiro.h"
#include "jogo/render.h"

void repl(Game *game)
{
  char info[256];
  while (game->cmd->type != CMD_EXIT)
  {
    render(game, info);

    if (await_command(game->game_ui.cmd_win, game->cmd_str))
    {
      strcpy(info, "Failed to read command.");
      continue;
    }

    if (parse_command(game))
    {
      strcpy(info, "Failed to parse command");
      continue;
    }

    if (game->cmd->track)
    {
      game->history = push_history(game->history, game->cmd);
    }

    bool cmd_invalid = game->cmd->type == CMD_INVALID;
    bool no_load_intent_while_no_tab = (game->cmd->type != CMD_LOAD && game->tabuleiro->data == NULL);
    if (cmd_invalid || no_load_intent_while_no_tab)
    {
      strcpy(info, "Invalid command.");
      continue;
    }

    if (run_command(game))
    {
      strcpy(info, "Failed to run command.");
      continue;
    }
    strcpy(info, "");
  }
}

int main(void)
{
  initscr();
  if (!has_colors())
  {
    endwin();
    printf("Your terminal does not support colors...\n");
    return 1;
  }

  init_colors();

  Game game;
  init_game(&game);

  repl(&game);

  // Nothing more to do, clean up.
  free_game(&game);
  endwin();
  return 0;
}
