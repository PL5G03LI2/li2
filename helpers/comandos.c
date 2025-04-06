#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../jogo/tabuleiro.h"
#include "comandos.h"
#include "strings.h"


int await_command(char *command) {
    if (!fgets(command, 256, stdin))
        return 0;
    command[strcspn(command, "\n")] = 0;
    return 1;
}

int parse_command(char *command) {
    ParsedCommand result = {CMD_INVALID, {{0}}, 0};
    
    if (strlen(command) == 0)
        return result.valid;
        
    switch(command[0]) {
        case 'g':
            if (sscanf(command, "g %s", result.args[0]) == 1) {
                result.type = CMD_SAVE;
                result.valid = 1;
            }
            break;
            
        case 'l':
            if (sscanf(command, "l %s", result.args[0]) == 1) {
                result.type = CMD_LOAD;
                result.valid = 1;
            }
            break;
            
        case 'b':
            if (strlen(command) > 1 && command[1] == ' ') {
            if (sscanf(command, "b %s", result.args[0]) == 1) {
                result.type = CMD_WHITE;
                result.valid = 1;
            }
            } else if (isdigit(command[1])) {
            strncpy(result.args[0], command, sizeof(result.args[0])-1);
            result.type = CMD_SELECT;
            result.valid = 1;
            }
            break;
            
        case 'r':
            if (sscanf(command, "r %s", result.args[0]) == 1) {
                result.type = CMD_CROSS;
                result.valid = 1;
            }
            break;
            
        case 'v':
            if (strlen(command) == 1) {
                result.type = CMD_VERIFY;
                result.valid = 1;
            }
            break;
            
        case 'a':
            if (strlen(command) == 1) {
                result.type = CMD_HELP;
                result.valid = 1;
            }
            break;
            
        case 'A':
            if (strlen(command) == 1) {
                result.type = CMD_HELP_ALL;
                result.valid = 1;
            }
            break;
            
        case 'R':
            if (strlen(command) == 1) {
                result.type = CMD_SOLVE;
                result.valid = 1;
            }
            break;
            
        case 'd':
            if (strlen(command) == 1) {
                result.type = CMD_UNDO;
                result.valid = 1;
            }
            break;
            
        case 's':
            if (strlen(command) == 1) {
                result.type = CMD_EXIT;
                result.valid = 1;
            }
            break;
            
        default:
            if (isalpha(command[0]) && isdigit(command[1])) {
                strncpy(result.args[0], command, sizeof(result.args[0])-1);
                result.type = CMD_SELECT;
                result.valid = 1;
            }
    }
    
    return result.valid;
}

int run_command(ParsedCommand cmd, Tab **tab) {
    switch(cmd.type) {
        case CMD_SAVE:
            return salvar_tabuleiro(*tab, cmd.args[0]);
            
        case CMD_LOAD:
            return carregar_tabuleiro(tab, cmd.args[0]);
            
        case CMD_WHITE: {
            int x = cmd.args[0][0] - 'a';
            int y = cmd.args[0][1] - '0';
            toggle_branco(*tab, x, y);
            return 1;
        }
            
        case CMD_CROSS: {
            int x = cmd.args[0][0] - 'a';
            int y = cmd.args[0][1] - '0';
            toggle_marked(*tab, x, y);
            return 1;
        }
            
        case CMD_VERIFY:
            return validar_tabuleiro(*tab);
            
        case CMD_EXIT:
            return 0;
            
        default:
            return 1;
    }
}

int carregar_tabuleiro(Tab **tab, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
        return 0;

    int height, width;
    fscanf(file, "%d %d", &height, &width);

    *tab = malloc(sizeof(Tab));
    (*tab)->height = height;
    (*tab)->width = width;
    (*tab)->data = malloc(height * width * sizeof(Piece));

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            fscanf(file, " %c", &(*tab)->data[i * width + j].c);
        }
    }

    fclose(file);
    return 1;
}

int salvar_tabuleiro(Tab *tab, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
        return 0;

    fprintf(file, "%d %d\n", tab->height, tab->width);
    for (int i = 0; i < tab->height; i++)
    {
        for (int j = 0; j < tab->width; j++)
        {
            fprintf(file, "%c", get_elem(tab, i, j));
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 1;
}

int validar_tabuleiro(Tab *tab)
{
    for (int i = 0; i < tab->height; i++)
    {
        for (int j = 0; j < tab->width; j++)
        {
            char c = get_elem(tab, i, j);

            if (c == '#')
                continue;

            for (int k = 0; k < tab->width; k++)
            {
                if (k != j && get_elem(tab, i, k) == c)
                    return 0;
            }

            for (int k = 0; k < tab->height; k++)
            {
                if (k != i && get_elem(tab, k, j) == c)
                    return 0;
            }
        }
    }
    
}
