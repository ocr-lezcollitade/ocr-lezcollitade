#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "solver.h"

// TESTS//

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

//

int *readGrid(char *inputFile, int DIM)
{

    if (DIM > 25
        || (DIM != 1 && DIM != 4 && DIM != 9 && DIM != 16 && DIM != 25))
    {
        printf("Dimension shouls be <= 25 and its square root should be an "
               "integer\n");
        return NULL;
    }

    FILE *fpointer = fopen(inputFile, "r");

    if (fpointer == NULL)
    {
        printf("Unable to open file %s\n", inputFile);
        return NULL;
    }

    char line[2 * DIM];

    int *grid = calloc(sizeof(int), DIM * DIM);

    if (grid == NULL)
    {
        printf("Unable to allocate space for the grid\n");
        return NULL;
    }

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
                    return NULL;
                }
                k++;
            }
            pk++;
        }
        if (pk > 0)
            i++;
    }

    fclose(fpointer);
    return grid;
}

void writeGrid(char *inputFile, int *grid, int DIM)
{

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
            if (grid[i * DIM + j] < 1)
            {
                ligne[pl] = '.';
                pl++;
            }
            else if (grid[i * DIM + j] < 10)
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

// SOLVING ALGORITHM //

static int isSafe(int *grid, int row, int col, int num, int DIM)
{

    // Check if we find the same num
    // in the similar row , we return 0
    for (int x = 0; x <= DIM - 1; x++)
        if (grid[row * DIM + x] == num)
            return 0;

    // Check if we find the same num in the
    // similar column , we return 0
    for (int x = 0; x <= DIM - 1; x++)
        if (grid[x * DIM + col] == num)
            return 0;

    // Check if we find the same num in the
    // particular DIM*DIM matrix, we return 0

    int sqrtDIM = sqrt(DIM);

    int startRow = row - row % sqrtDIM, startCol = col - col % sqrtDIM;

    for (int i = 0; i < sqrtDIM; i++)
        for (int j = 0; j < sqrtDIM; j++)
            if (grid[(i + startRow) * DIM + (j + startCol)] == num)
                return 0;

    return 1;
}

static int solveSudoku(int *grid, int row, int col, int DIM)
{

    if (row == DIM - 1 && col == DIM)
        return 1;

    if (col == DIM)
    {
        row++;
        col = 0;
    }

    if (grid[row * DIM + col] > 0)
        return solveSudoku(grid, row, col + 1, DIM);

    for (int num = 1; num <= DIM; num++)
    {

        if (isSafe(grid, row, col, num, DIM) == 1)
        {
            grid[row * DIM + col] = num;

            if (solveSudoku(grid, row, col + 1, DIM) == 1)
                return 1;
        }
        grid[row * DIM + col] = 0;
    }
    return 0;
}

// END OF SOLVING PART //

int solve(int *grid, int DIM)
{

    if (grid == NULL)
        return 0;

    // Si mauvaise grille

    if (!checkCol(grid, DIM) || !checkRow(grid, DIM)
        || !checkSquare(grid, DIM))
    {
        printf("Error in the initial grid :(\n");
        return 0;
    }

    if (solveSudoku(grid, 0, 0, DIM) == 1)
    {
        printf("Successfully solved the grid\n");
        return 1;
    }
    printf("Not able to solve the grid :(\n");
    return 0;
}

int *copy_grid(int *grid, int DIM)
{

    int *copy = calloc(DIM, sizeof(int));

    if (copy == NULL)
        return NULL;

    for (int i = 0; i < DIM; i++)
        copy[i] = grid[i];

    return copy;
}
