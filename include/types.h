#ifndef TYPES_H
#define TYPES_H

#include <curses.h>
#include <stdbool.h>

typedef struct
{
    int x;
    int y;
} iVec2;

typedef struct
{
    char c;
    bool marked;
    bool violated;
} Piece;

typedef struct
{
    Piece *data;
    iVec2 sel_piece;
    int height;
    int width;
} Tab;

typedef enum
{
    CMD_INVALID = 0,
    CMD_SAVE,     // g <filename>
    CMD_LOAD,     // l <filename>
    CMD_SELECT,   // <coord>
    CMD_WHITE,    // b <coord>
    CMD_WHITE_NON_OBSTACLE, // t
    CMD_CROSS,    // r <coord>
    CMD_VERIFY,   // v
    CMD_HELP,     // a
    CMD_HELP_ALL, // A
    CMD_SOLVE,    // R
    CMD_UNDO,     // d
    CMD_EXIT,     // s
    CMD_CONTINUE
} CommandType;

typedef struct
{
    CommandType type;
    char **tokens;
    bool track;
} ParsedCommand;

typedef struct node
{
    ParsedCommand *cmd;
    struct node *next;
} TabHistory;

typedef struct window
{
    WINDOW *win;
    iVec2 pos;
    iVec2 size;
} WIN;

typedef struct ui
{
    WIN main_win;
    WIN help_win;
    WIN cmd_win;
    iVec2 offsets;
} UI;

typedef struct game
{
    Tab *tabuleiro;
    TabHistory *history;
    char *cmd_str;
    ParsedCommand *cmd;
    iVec2 win_d;
    char *save_to;

    char *info_str;

    UI game_ui;
} Game;

#endif
