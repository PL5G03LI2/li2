#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "helpers/history.h"
#include "helpers/memory.h"
#include "helpers/strs.h"
#include "jogo/comandos.h"
#include "jogo/tabuleiro.h"

#define HELP_TABLE_WIDTH 42
#define HELP_TABLE_HEIGHT 15

static void init_colors(void) {
    start_color();
    use_default_colors();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_RED, COLOR_WHITE);
    init_pair(3, COLOR_BLACK, COLOR_RED);
    init_pair(4, COLOR_WHITE, -1);
}

void draw_box(int y, int x, int height, int width) {
    mvaddch(y, x, ACS_ULCORNER);
    for(int i = 1; i < width-1; i++) mvaddch(y, x+i, ACS_HLINE);
    mvaddch(y, x+width-1, ACS_URCORNER);
    
    for(int i = 1; i < height-1; i++) {
        mvaddch(y+i, x, ACS_VLINE);
        mvaddch(y+i, x+width-1, ACS_VLINE);
    }
    
    mvaddch(y+height-1, x, ACS_LLCORNER);
    for(int i = 1; i < width-1; i++) mvaddch(y+height-1, x+i, ACS_HLINE);
    mvaddch(y+height-1, x+width-1, ACS_LRCORNER);
}

void print_help_table(int start_y, int start_x) {
    const char *commands[] = {
        "g <file>", "l <file>", "<coord>", 
        "b <coord>", "r <coord>", "v", 
        "a", "A", "R", "d", "s", NULL
    };
    
    const char *descriptions[] = {
        "Save game state",
        "Load game state",
        "Format: letter+number",
        "Make letter uppercase",
        "Place # in position",
        "Verify constraints",
        "Auto-help (once)",
        "Continuous auto-help",
        "Solve puzzle",
        "Undo last command",
        "Exit program",
        NULL
    };

    attron(COLOR_PAIR(4));
    draw_box(start_y, start_x, HELP_TABLE_HEIGHT, HELP_TABLE_WIDTH);
    mvprintw(start_y+1, start_x + (HELP_TABLE_WIDTH/2) - 10, " Available Commands ");
    
    for(int i = 0; commands[i] && descriptions[i]; i++) {
        mvprintw(start_y+3+i, start_x + 2, "%-12s %s", commands[i], descriptions[i]);
    }
    
    attroff(COLOR_PAIR(4));
}

void calculate_layout(Game *game, int *board_y, int *board_x, int *help_y, int *help_x) {
    if(!game->tabuleiro || !game->tabuleiro->data) return;

    int board_h = 3 + game->tabuleiro->height;
    int board_w = 3 + 2 * game->tabuleiro->width;
    int min_required_h = board_h + HELP_TABLE_HEIGHT + 4;
    int min_required_w = board_w + HELP_TABLE_WIDTH + 4;

    *board_x = (game->win_d.x - board_w) / 2;
    *board_y = 2;
    
    if(game->win_d.x >= min_required_w && game->win_d.y >= min_required_h) {
        *help_x = *board_x + board_w + 4;
        *help_y = *board_y;
    } else {
        *help_x = (game->win_d.x - HELP_TABLE_WIDTH) / 2;
        *help_y = *board_y + board_h + 2;
        
        if(*help_y + HELP_TABLE_HEIGHT > game->win_d.y - 3) {
            *help_y = *help_x = -1;
        }
    }
}

void render(Game *game, char *info) {
    clear();
    print_info(info, game->win_d);

    if(game->tabuleiro && game->tabuleiro->data) {
        int board_y, board_x, help_y, help_x;
        calculate_layout(game, &board_y, &board_x, &help_y, &help_x);

        print_tab(game->tabuleiro, game->win_d);
        
        if(help_y > 0 && help_x > 0 && 
           help_y + HELP_TABLE_HEIGHT < game->win_d.y - 2 &&
           help_x + HELP_TABLE_WIDTH < game->win_d.x) {
            print_help_table(help_y, help_x);
        }
    } else {
        const char *msg = "Awaiting load command... Hint: l <save_file>";
        mvprintw(game->win_d.y/2, (game->win_d.x - strlen(msg))/2, "%s", msg);
    }

    move(game->win_d.y - 1, 0);
    clrtoeol();
    refresh();
}

void repl(Game *game) {
    char info[256] = {0};
    while(game->cmd->type != CMD_EXIT) {
        getmaxyx(stdscr, game->win_d.y, game->win_d.x);
        render(game, info);

        if(await_command(game->cmd_str)) {
            strcpy(info, "Input error");
            continue;
        }

        if(parse_command(game)) {
            strcpy(info, "Invalid command");
            continue;
        }

        if(game->cmd->track) {
            game->history = push_history(game->history, game->cmd);
        }

        bool valid_state = game->tabuleiro && game->tabuleiro->data;
        if(game->cmd->type == CMD_INVALID || (!valid_state && game->cmd->type != CMD_LOAD)) {
            strcpy(info, "Invalid command");
            continue;
        }

        if(run_command(game)) {
            strcpy(info, "Execution failed");
            continue;
        }
        
        info[0] = '\0';
    }
}

int main(void) {
  initscr();
  cbreak();
  noecho();  
  keypad(stdscr, TRUE);
  curs_set(1);

    if(!has_colors()) {
        endwin();
        printf("Terminal lacks color support\n");
        return 1;
    }

    init_colors();

    Game game;
    init_game(&game);

    repl(&game);

    free_game(&game);
    endwin();
    return 0;
}