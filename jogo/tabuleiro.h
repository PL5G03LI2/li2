struct Tab
{
    char *data;
    int height;
    int width;
};

/*
 Ensures a valid position.

 @returns 1 if position is valid.
 @returns 0 otherwise.
*/
int assert_pos(struct Tab *tabuleiro, int x, int y);

/*
 Retorna o char na coordenada (x, y)
*/
char get_pos(struct Tab *tabuleiro, int x, int y);

/*
 Set's the given character, c, at the position in the board
*/
void set_pos(struct Tab *tabuleiro, int x, int y, char c);

/*
 Set a position to white letter (Uppercase)
*/
void set_branco(struct Tab *tabuleiro, int x, int y);

/*
 Imprime o tabuleiro para o ecrã
*/
void print_tabuleiro(struct Tab *tabuleiro, int width, int height);
