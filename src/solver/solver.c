#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "solver.h"

struct Cell
{
    int x;
    int y;
};

int checkRow(int grid[9][9])
{
    for (int i = 0; i < 9; i++)
    {
        int histo[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        for (int j = 0; j < 9; j++)
        {
            histo[grid[i][j]]++;
        }
        for (int k = 1; k < 10; k++)
        {
            if (histo[k] > 1)
                return 0;
        }
    }
    return 1;
}

int checkCol(int grid[9][9])
{
    for (int i = 0; i < 9; i++)
    {
        int histo[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        for (int j = 0; j < 9; j++)
        {
            histo[grid[j][i]]++;
        }
        for (int k = 1; k < 10; k++)
        {
            if (histo[k] > 1)
                return 0;
        }
    }
    return 1;
}

int checkSquare(int grid[9][9])
{
    for (int a = 0; a < 9 / 3; a++)
    {
        for (int b = 0; b < 9 / 3; b++)
        {
            int histo[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            for (int i = 0; i < 9 / 3; i++)
            {
                for (int j = 0; j < 9 / 3; j++)
                {
                    histo[grid[a * 9 / 3 + i][b * 9 / 3 + j]]++;
                }
                for (int k = 1; k < 10; k++)
                {
                    if (histo[k] > 1)
                        return 0;
                }
            }
        }
    }
    return 1;
}

int allChecks(int grid[9][9])
{
    return checkSquare(grid) + checkCol(grid) + checkRow(grid) == 3;
}

int win(int grid[9][9])
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (grid[i][j] == 0)
                return 0;
        }
    }
    return checkSquare(grid) + checkCol(grid) + checkRow(grid) == 3;
}

int solve(char *inputFile)
{

    FILE *fpointer = fopen(inputFile, "r");
    char line[15];

    int grid[9][9];
    int i = 0;
    int delta = 0;

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            grid[i][j] = 0;
        }
    }

    while (fgets(line, 15, fpointer))
    {
        if (i != 3 && i != 7)
        {
            for (int j = 0; j < 3; j++)
            {
                if (line[j] >= '0' && line[j] <= '9')
                    grid[i - delta][j] = line[j] - 48;
            }
            for (int j = 4; j < 7; j++)
            {
                if (line[j] >= '0' && line[j] <= '9')
                    grid[i - delta][j - 1] = line[j] - 48;
            }
            for (int j = 8; j < 11; j++)
            {
                if (line[j] >= '0' && line[j] <= '9')
                    grid[i - delta][j - 2] = line[j] - 48;
            }
        }
        else if (i == 3)
            delta = 1;
        else if (i == 7)
            delta = 2;
        i++;
    }

    fclose(fpointer);

    // Debug

    /*for(int i=0;i<9;i++){
        for(int j=0;j<9;j++){
        printf("%d",grid[i][j]);
        }
        printf("\n");
    }

    //    printf("CheckRow : %d\n",checkRow(grid));
    //    printf("CheckCol : %d\n",checkCol(grid));
    //    printf("CheckSquare : %d\n",checkSquare(grid));

    */

    // Si mauvaise grille

    if (!checkCol(grid) || !checkRow(grid) || !checkSquare(grid))
    {
        printf("Erreur dans grille d'entrée\n");
        return 1;
    }

    // On defini les case vides

    int nbEmpty = 0;
    struct Cell empty[81];

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (grid[i][j] == 0)
            {
                struct Cell c;
                c.x = i;
                c.y = j;
                empty[nbEmpty] = c;
                nbEmpty++;
            }
        }
    }

    // Debut de la boucle principale

    int index = 0;

    while (index < nbEmpty && index >= 0)
    {
        struct Cell curr = empty[index];
        if (grid[curr.x][curr.y] < 9)
        {
            grid[curr.x][curr.y]++;
            if (allChecks(grid))
                index++;
        }
        else
        {
            grid[curr.x][curr.y] = 0;
            index--;
        }
    }

    if (index < 0)
    {
        printf("Pas résolvable :(\n");
        return 0;
    }

    if (win(grid))
    {
        /*printf("Voici la solution\n");
        for(int i=0;i<9;i++){
            for(int j=0;j<9;j++){
                printf("%d",grid[i][j]);
            }
            printf("\n");
        }*/

        // On ecrit dans un nouveau file
        char outputExt[] = ".result";
        strcat(inputFile, outputExt);
        FILE *pfile = fopen(inputFile, "w");

        char ligne[12];
        ;
        for (int i = 0; i < 9; i++)
        {
            int d = 0;
            for (int j = 0; j < 9; j++)
            {
                ligne[j + d] = grid[i][j] + 48;
                if (j == 2 || j == 5)
                {
                    d++;
                    ligne[j + d] = ' ';
                }
            }
            ligne[11] = '\0';
            // for(int k=0;k<12;k++) printf("%c",ligne[k]);
            // printf("fin\n");
            fputs(ligne, pfile);
            // char test[3] = {i+48,'\n','\0'};
            fputs("\n", pfile);
            if (i == 2 || i == 5)
                fputs("\n", pfile);
            // free(ligne);
        }
        fclose(pfile);
    }
    return 0;
}
