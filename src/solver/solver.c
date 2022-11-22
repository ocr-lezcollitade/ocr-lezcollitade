#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "solver.h"

struct Cell
{
    int x;
    int y;
};

int checkRow(int *grid, int DIM)
{
    for (int i = 0; i < DIM; i++)
    {
        int *histo = calloc(DIM + 1, sizeof(int));
        for (int j = 0; j < DIM; j++)
        {
            histo[grid[i * DIM + j]]++;
        }
        for (int k = 1; k < DIM + 1; k++)
        {
            if (histo[k] > 1)
            {
                free(histo);
                return 0;
            }
        }
        free(histo);
    }
    return 1;
}

int checkCol(int *grid, int DIM)
{
    for (int i = 0; i < DIM; i++)
    {
        int *histo = calloc(DIM + 1, sizeof(int));
        for (int j = 0; j < DIM; j++)
        {
            histo[grid[j * DIM + i]]++;
        }
        for (int k = 1; k < DIM + 1; k++)
        {
            if (histo[k] > 1)
            {
                free(histo);
                return 0;
            }
        }
        free(histo);
    }
    return 1;
}

int checkSquare(int *grid, int DIM)
{
    int sqDIM = sqrt(DIM);
    for (int a = 0; a < DIM / sqDIM; a++)
    {
        for (int b = 0; b < DIM / sqDIM; b++)
        {
            int *histo = calloc(DIM + 1, sizeof(int));
            for (int i = 0; i < DIM / sqDIM; i++)
            {
                for (int j = 0; j < DIM / sqDIM; j++)
                {
                    histo[grid[(a * DIM / sqDIM + i) * DIM
                               + (b * DIM / sqDIM + j)]]++;
                }
                for (int k = 1; k < DIM + 1; k++)
                {
                    if (histo[k] > 1)
                    {
                        free(histo);
                        return 0;
                    }
                }
            }
            free(histo);
        }
    }
    return 1;
}

int allChecks(int *grid, int DIM)
{
    return checkSquare(grid, DIM) + checkCol(grid, DIM) + checkRow(grid, DIM)
           == 3;
}

int win(int *grid, int DIM)
{
    for (int i = 0; i < DIM; i++)
    {
        for (int j = 0; j < DIM; j++)
        {
            if (grid[i * DIM + j] == 0)
                return 0;
        }
    }
    return checkSquare(grid, DIM) + checkCol(grid, DIM) + checkRow(grid, DIM)
           == 3;
}

int solve(char *inputFile, int DIM)
{
    if (DIM > 25
        || (DIM != 1 && DIM != 4 && DIM != 9 && DIM != 16 && DIM != 25))
    {
        printf("Dimension shouls be <= 25 and its square root should be an "
               "integer");
        return 1;
    }

    FILE *fpointer = fopen(inputFile, "r");
    char line[2 * DIM];

    int *grid = calloc(sizeof(int), DIM * DIM);
    int i = 0;

    while (fgets(line, 2 * DIM, fpointer))
    {

        size_t k = 0, pk = 0;
        while (line[pk] != '\n')
        {

            if (line[pk] != ' ')
            {
                if (line[pk] >= '1' && line[pk] <= '9')
                    grid[i * DIM + k] = line[pk] - 48;
                else if (line[pk] >= 'A' && line[pk] <= 'Z')
                    grid[i * DIM + k] = line[pk] - 55;
                else if (line[pk] != '.')
                {
                    printf("Unknown charater %c\n", line[pk]);
                    return 1;
                }
                k++;
            }
            pk++;
        }
        if (pk > 0)
            i++;
    }

    fclose(fpointer);

    // Debug

    /*
    for(int i=0;i<DIM;i++){
        for(int j=0;j<DIM;j++){
        printf("%d.",grid[i][j]);
        }
        printf("\n");
    }
    //    printf("CheckRow : %d\n",checkRow(grid));
    //    printf("CheckCol : %d\n",checkCol(grid));
    //    printf("CheckSquare : %d\n",checkSquare(grid));

    */

    // Si mauvaise grille

    if (!checkCol(grid, DIM) || !checkRow(grid, DIM)
        || !checkSquare(grid, DIM))
    {
        printf("Erreur dans grille d'entrée\n");
        return 1;
    }

    // On defini les case vides

    int nbEmpty = 0;
    struct Cell empty[DIM * DIM];

    for (int i = 0; i < DIM; i++)
    {
        for (int j = 0; j < DIM; j++)
        {
            if (grid[i * DIM + j] == 0)
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
        if (grid[curr.x * DIM + curr.y] < DIM)
        {
            grid[curr.x * DIM + curr.y]++;
            if (allChecks(grid, DIM))
                index++;
        }
        else
        {
            grid[curr.x * DIM + curr.y] = 0;
            index--;
        }
    }

    if (index < 0)
    {
        printf("Pas résolvable :(\n");
        return 0;
    }

    if (win(grid, DIM))
    {
        /*
        printf("Voici la solution\n");
        for(int i=0;i<DIM;i++){
            for(int j=0;j<DIM;j++){
                printf("%d.",grid[i][j]);
            }
            printf("\n");
        }*/

        // On ecrit dans un nouveau file
        char outputExt[] = ".result";
        strcat(inputFile, outputExt);
        FILE *pfile = fopen(inputFile, "w");

        char ligne[2 * DIM];

        size_t mod = sqrt(DIM);

        for (int i = 0; i < DIM; i++)
        {
            if (i > 0 && i % mod == 0)
                fputs("\n", pfile);

            size_t pl = 0;
            for (int j = 0; j < DIM; j++)
            {
                if (j > 0 && j % mod == 0)
                {
                    ligne[pl] = ' ';
                    pl++;
                }

                if (grid[i * DIM + j] < 10)
                {
                    ligne[pl] = grid[i * DIM + j] + 48;
                    pl++;
                }
                else
                {
                    ligne[pl] = grid[i * DIM + j] + 55;
                    pl++;
                }
            }
            ligne[pl] = '\0';
            fputs(ligne, pfile);
            fputs("\n", pfile);
        }
        fclose(pfile);
    }

    free(grid);

    return 0;
}
