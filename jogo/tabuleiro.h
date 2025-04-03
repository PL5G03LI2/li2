typedef struct
{
    int x;
    int y;
} iVec2;

typedef struct
{
    char c;
    int marked;
} Piece;

typedef struct
{
    Piece *data;
    int height;
    int width;
} Tab;

/*
 Calculates the index of the position provided

 @returns 0 <= index <= height * width;
 @returns -1 if invalid;
*/
int calc_index(Tab *tabuleiro, int x, int y);

/*
 Ensures valid string index.

 @returns 1 if index is in bounds.
 @returns 0 otherwise.
 */
int assert_index(Tab *tabuleiro, int i);

/*
 Ensures a valid position.

 @returns 1 if position is valid.
 @returns 0 otherwise.
*/
int assert_pos(Tab *tabuleiro, int x, int y);

/*
 Retorna o char na coordenada (x, y)
*/
char get_elem(Tab *tabuleiro, int x, int y);

/*
 Set's the given character, c, at the position in the board

 @note Has no effect if coords are out of bounds.
*/
void set_elem(Tab *tabuleiro, int x, int y, char c);

/*
 Set a position to white letter (Uppercase)
*/
void toggle_branco(Tab *tabuleiro, int x, int y);

/*
 Imprime o tabuleiro para o ecrã
*/
void print_tabuleiro(Tab *tabuleiro, int width, int height);

/*
 Toggle marked status of a piece and toggles branco of all ortogonal neighbours
*/
void toggle_marked(Tab *tabuleiro, int x, int y);
