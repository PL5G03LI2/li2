typedef struct tabuleiro
{
    char *data;
    int height;
    int width;
} Tab;

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
