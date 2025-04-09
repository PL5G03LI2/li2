#ifndef TYPES_H
#define TYPES_H

typedef enum
{
    false = 0,
    true = 1
} bool;

typedef struct
{
    int x;
    int y;
} iVec2;

typedef struct
{
    char c;
    bool marked;
} Piece;

typedef struct
{
    Piece *data;
    iVec2 sel_piece;
    int height;
    int width;
} Tab;

typedef struct node
{
    ParsedCommand cmd;
    struct node *next;
} TabHistory;

typedef enum
{
    CMD_INVALID = 0,
    CMD_SAVE,     // g <filename>
    CMD_LOAD,     // l <filename>
    CMD_SELECT,   // <coord>
    CMD_WHITE,    // b <coord>
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
} ParsedCommand;
#endif
