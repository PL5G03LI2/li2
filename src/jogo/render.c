#include <curses.h>

#include "jogo/render.h"
#include "types.h"

void init_colors(void)
{
    start_color();
    use_default_colors();
    init_pair(1, COLOR_BLACK, COLOR_WHITE); // selected normal
    init_pair(2, COLOR_RED, COLOR_WHITE);   // selected violated
    init_pair(3, COLOR_BLACK, COLOR_RED);   // violated not selected
    init_pair(4, COLOR_WHITE, -1);          // default
}

void calculate_layout(Game *game)
{
    int maxy = 0, maxx = 0;
    getmaxyx(stdscr, maxy, maxx);

    if (!maxy || !maxx)
        return;

    game->win_d = (iVec2){maxx, maxy};

    game->game_ui.main_win.pos = (iVec2){0, 0};

    // small screen
    if (maxx < 85)
    {
        game->game_ui.main_win.size = (iVec2){maxx, maxy - 3};
        game->game_ui.offsets.x += maxx;
        game->game_ui.offsets.y += maxy - 3;

        // move offscreen, hides it
        game->game_ui.help_win.size = (iVec2){0, 0};
        game->game_ui.help_win.pos.x = 90;
    }
    // big screen, take in thirds
    else
    {
        game->game_ui.main_win.size = (iVec2){maxx / 3 * 2, maxy - 3};
        game->game_ui.offsets = game->game_ui.main_win.size;

        game->game_ui.help_win.pos.x = game->game_ui.offsets.x;
        game->game_ui.help_win.size = (iVec2){maxx / 3, maxy - 3};
    }

    game->game_ui.cmd_win.size = (iVec2){maxx, 3};
    game->game_ui.cmd_win.pos = (iVec2){0, maxy - 3};
}

void apply_layout(UI *ui)
{
    WIN main = ui->main_win;
    wresize(main.win, main.size.y, main.size.x);
    mvwin(main.win, main.pos.y, main.pos.x);

    WIN help = ui->help_win;
    wresize(help.win, help.size.y, help.size.x);
    mvwin(help.win, help.pos.y, help.pos.x);

    WIN cmd = ui->cmd_win;
    wresize(cmd.win, cmd.size.y, cmd.size.x);
    mvwin(cmd.win, cmd.pos.y, cmd.pos.x);
}

void print_info(WIN cmd_win, const char *string)
{
    wmove(cmd_win.win, 0, 2);
    if (string[0] != '\0')
        wprintw(cmd_win.win, "[ CMD: %s ]", string);
    else
        wprintw(cmd_win.win, "[ CMD ]");
}

void print_tab(Game *game)
{
    WIN main = game->game_ui.main_win;
    Tab *tab = game->tabuleiro;

    int h = tab->height, w = tab->width;
    int board_h = 3 + h;                       // Height: rows + header + separator
    int board_w = 3 + 2 * w;                   // Width: 2 chars per cell + padding
    int start_y = (main.size.y - board_h) / 2; // Vertical centering
    int start_x = (main.size.x - board_w) / 2; // Horizontal centering

    if (main.size.x - 2 < board_w || main.size.y - 2 < board_h)
    {
        mvwprintw(main.win, main.size.y / 2, main.size.x / 2 - 8, "Window too small.");
        return;
    }

    // Column headers
    attron(COLOR_PAIR(4));
    mvwprintw(main.win, start_y, start_x + 1, " ");
    for (int y = 0; y < w; y++)
        wprintw(main.win, "%d ", y + 1);
    attroff(COLOR_PAIR(4));

    // Separator (uses default color)
    mvwprintw(main.win, start_y + 1, start_x, "--");
    for (int y = 0; y < w; y++)
        wprintw(main.win, "--");

    // Rows
    for (int x = 0; x < h; x++)
    {
        // Row Header (e.g., "a|")
        attron(COLOR_PAIR(4));
        mvwprintw(main.win, start_y + 2 + x, start_x, "%c|", 'a' + x); // "a|", "b|", etc.
        attroff(COLOR_PAIR(4));

        // Cells in the Row
        for (int y = 0; y < w; y++)
        {
            int idx = x * w + y;
            Piece p = tab->data[idx];
            bool sel = (x == tab->sel_piece.x && y == tab->sel_piece.y);

            // Color Logic
            int cp = 4; // Default: white text, default background
            if (p.violated)
                cp = sel ? 2 : 3; // Violated cells
            else if (sel)
                cp = 1; // Selected cell

            // Print Cell
            attron(COLOR_PAIR(cp));
            mvwprintw(main.win, start_y + 2 + x, start_x + 2 + 2 * y, "%c", p.marked ? '#' : p.c);
            attroff(COLOR_PAIR(cp));
        }
    }
}

void render_main(Game *game)
{
    WIN main = game->game_ui.main_win;
    werase(main.win);
    box(main.win, 0, 0);

    wmove(main.win, 0, 2);
    wprintw(main.win, "[ Board ]");

    if (game->tabuleiro->data)
    {
        print_tab(game);
    }
    else
    {
        wmove(main.win, main.size.y / 2, main.size.x / 2 - 22);
        wprintw(main.win, "Awaiting load command... Hint: l <save_file>");
    }

    wrefresh(main.win);
}

void render_help(Game *game)
{
    WIN help = game->game_ui.help_win;
    werase(help.win);
    if (help.size.x == 0 || help.size.y == 0)
        return;
    box(help.win, 0, 0);

    wmove(help.win, 0, 2);
    wprintw(help.win, "[ Help ]");

    const char *commands[11] = {
        "g <file>", "l <file>", "<coord>",
        "b <coord>", "r <coord>", "v",
        "a", "A", "R", "d", "s"};

    const char *descriptions[11] = {
        "Save",
        "Load",
        "<letter><number>",
        "Toggle white",
        "Cross",
        "Verify",
        "Help (once)",
        "Help all",
        "Solve",
        "Undo",
        "Exit",
    };

    for (int i = 0; i < 11; i++)
    {
        wmove(help.win, 2 + i, 2);
        wprintw(help.win, "%s %s", commands[i], descriptions[i]);
    }

    wrefresh(help.win);
}

void render_cmd(Game *game, char *info)
{
    WIN cmd = game->game_ui.cmd_win;
    werase(cmd.win);
    box(cmd.win, 0, 0);

    print_info(cmd, info);

    wmove(cmd.win, 1, 1);
    wrefresh(cmd.win);
}

void render(Game *game, char *info)
{
    curs_set(0);
    clear();
    calculate_layout(game);
    apply_layout(&game->game_ui);

    render_main(game);
    render_help(game);
    render_cmd(game, info);

    curs_set(1);
}