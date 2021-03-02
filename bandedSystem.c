/* -- bandedSystem.c -- this program solves the banded matrix solutions
 *
 * It is intentionally written in C89 for portability purposes.
 */

#include "bandedSystem.h"
/*#include "file_io.h"*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>

int main(int argc, char* argv[])
{

    size_t matSize = 4;

    /* Fill dem all with all the zeros */

    double* bandedMatrix = (double*)calloc(matSize * matSize, sizeof(double));
    double* workMatrix = (double*)calloc(matSize * matSize, sizeof(double));
    double* answers = (double*)calloc(matSize, sizeof(double));

    if (bandedMatrix == NULL || answers == NULL || workMatrix == NULL)
        return -1;

    /* This block fills the cells with the length of the martix */
    {
        size_t wid = 0, len = 0;
        size_t maxSeekW = matSize, maxSeekH = matSize;

        while (wid < maxSeekW && len < maxSeekH) /* This condition prevents buffer overflow */
        {
            bandedMatrix[wid + (matSize * len)] = (double)matSize;
            wid++, len++;
        }
    }

    /* This block fills the cells next to the diagonal matrix with 1 */
    {
        size_t seek = 0, maxSeek = matSize - 1;
        
        while (seek < maxSeek)
        {
            bandedMatrix[(seek + 1) + (matSize * seek)] = 1;
            bandedMatrix[seek + (matSize * (seek+1))] = 1;
            seek++;
        }
    }
    for (size_t y = 0; y < matSize; y++)
    {
        for (size_t x = 0; x < matSize; x++)
            printf("%f ", bandedMatrix[x + (matSize * y)]);
        printf("\n");
    }
    
    /* Solves the banded matrix */

    memcpy(workMatrix, bandedMatrix, matSize * matSize * sizeof(double));

    /*for (size_t j = 0; j < matSize; j++)
    {
        size_t m = 
        printf();

    }*/

    /* Export result CSV */

    FILE* solvedTable;

    size_t serialNumber = 0;

    char numStr = (char*)calloc(4096, sizeof(char)); /* Big buffer to prevent overflow */
    if (!numStr)

    while (serialNumber < SIZE_MAX)
    {
        
        snprintf(numStr, "result_%lu.csv", 511, (size_t)serialNumber);

        solvedTable = fopen(numStr, "r");
        if (solvedTable)
        {
                fclose(solvedTable);
                serialNumber++;
        }
        else
        {
            solvedTable = fopen(numStr, "w");
            if (solvedTable)
            {
                for (size_t i = 0; i < matSize; i++)
                fprintf(solvedTable, "X_%lu;", i);

                fprintf(solvedTable, "Answer; Y;\n");


                /* O(n^2) -- This loop writes the results of solving the equation to the CSV file -- */
                for (size_t x = 0; x < matSize; x++)
                {
                    for (size_t y = 0; y < matSize; y++)
                    {
                        fprintf(solvedTable, "%lu;", bandedMatrix[x + (x*y)]);
                    }

                    fprintf(solvedTable, "%lu;Y_%lu;\n", answers[x], x);
                }

                fclose(solvedTable);
                break;  
            }
            else
            {
                printf("Cannot open %s", numStr);
                break;
            }
        }

    }

    free(bandedMatrix);
    free(answers);
    free(workMatrix);
    free(numStr);

    return 0;
}