/* -- bandedSystem.c -- this program solves the banded matrix solutions
 *
 * It is intentionally written in C89 for portability purposes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>

typedef 
enum errorCode {
    NOMINAL_RUN = 0,
    ALLOCATION_ERROR = 1,
    ZERO_ERROR = 2
} errorCode;

int main(int argc, char* argv[])
{
    /*
        Input: banded 4x4 system (Part 1)
        Output: solution vector x=(1 2 3 4)
    */

    FILE *debugFile = fopen("test.log","w");
    if (!debugFile)
        return ALLOCATION_ERROR;

    size_t matSize = 4; // constant value for testing purposes
    const size_t matSizeM1 = matSize - 1;

    if (matSize == 0)
        return ZERO_ERROR;

    /* Fill dem all with all the zeros */

    double* bandedMatrix = (double*)calloc(matSize * matSize, sizeof(double));
    double* workMatrix = (double*)calloc(matSize * matSize, sizeof(double));
    double constants[4] = {6.0, 12.0, 18.0, 19.0}; // hardcoded constants for testing
    double* workConstants = (double*)calloc(matSize, sizeof(double));
    double* answers = (double*)calloc(matSize, sizeof(double));
    

    if (bandedMatrix == NULL || workMatrix == NULL || workConstants == NULL || answers == NULL)
        return ALLOCATION_ERROR;


    for (size_t pos = 0; pos < 4; pos++)
    {
        workConstants[pos] = constants[pos];
        fprintf(debugFile, "%f\n", workConstants[pos]);
    }

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
        size_t seek = 0, maxSeek = matSizeM1;
        
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
            fprintf(debugFile, "%f ", bandedMatrix[x + (matSize * y)]);
        fprintf(debugFile, "\n");
    }
    fprintf(debugFile, "\n");
    /* Solves the banded matrix */

    /*for j in range(1,n):
        m= c[j]/a[j-1]
        a[j] -= m*d[j-1]
        b[j] -= m*b[j-1]

    x[n-1]=b[n-1]/a[n-1]
    for j in range(2,-1-1):
        x[j] = (b[j]-d[j]*x[j+1])/a[j]
    end
    */

    memcpy(workMatrix, bandedMatrix, matSize * matSize * sizeof(double));

    if (matSize == 1)
    {
        answers[0] = workConstants[0] / workMatrix[0];
    }
    else
    {
        for (size_t seek = 0; seek < matSizeM1; seek++)
        {
            double m = workMatrix[seek + ((seek+1) * matSize)] / workMatrix[seek + (seek * matSize)];

            workMatrix[(seek + 1) + ((seek + 1) * matSize)] -= (m * workMatrix[(seek+1) + (seek * matSize)]);

            workConstants[seek + 1] -= (m * workConstants[seek]);

        }

        for (size_t y = 0; y < matSize; y++)
        {
            for (size_t x = 0; x < matSize; x++)
                fprintf(debugFile, "%f ", workMatrix[x + (matSize * y)]);
            fprintf(debugFile, "\n");
        }

        /* Back solving */

        for (size_t seek = matSize; seek > 0; seek--) /* seek > 0 prevents underflow */
        {
            const size_t seekM1 = seek - 1;
            
            if (seek == matSize)
            {
                answers[seekM1] = constants[seekM1] / workMatrix[(matSize * matSize) - 1];
                fprintf(debugFile, "%f / %f = %f\n", constants[seekM1], workMatrix[(matSize * matSize) - 1], answers[seekM1]);
            }
            else
            {
                const size_t v = seekM1 + (seekM1 * matSize);
                answers[seekM1] =
                    (workConstants[seekM1] - workMatrix[seekM1 + (seekM1 * matSize)] * answers[seek]) /
                    workMatrix[seek + (seekM1 * matSize)];
                fprintf(debugFile, "(%f - %f * %f) / %f = %f\n\n", workConstants[seekM1], workMatrix[seekM1 + (seekM1 * matSize)], answers[seek], workMatrix[seek + (seekM1 * matSize)], answers[seekM1]);
            }
        }
    }

    fclose(debugFile);

    /* Export result CSV */

    FILE* solvedTable;

    size_t serialNumber = 0;

    char* numStr = (char*)calloc(4096, sizeof(char)); /* Big buffer to prevent overflow */
    if (!numStr)
    {
        printf("Cannot allocate space for directory name");
        free(bandedMatrix);
        free(workMatrix);
        free(workConstants);
        free(answers);

        return ALLOCATION_ERROR;
    }

    while (serialNumber < SIZE_MAX)
    {
        
        snprintf((const char*)numStr, 511, "result_%lu.csv", (size_t)serialNumber);

        solvedTable = fopen((const char*)numStr, "r");
        if (solvedTable)
        {
            fclose(solvedTable);
            serialNumber++;
        }
        else
        {
            solvedTable = fopen((const char*)numStr, "w");
            if (solvedTable)
            {
                for (size_t i = 0; i < matSize; i++)
                    fprintf(solvedTable, "X_%llu,", i);

                fprintf(solvedTable, "Answer, Y,\n");


                /* O(n^2) -- This loop writes the results of solving the equation to the CSV file -- */
                for (size_t x = 0; x < matSize; x++)
                {
                    for (size_t y = 0; y < matSize; y++)
                    {
                        fprintf(solvedTable, "%f,", bandedMatrix[x + (matSize * y)]);
                    }

                    fprintf(solvedTable, "%f,Y_%lu,\n", answers[x], x);
                }
                printf("Writing to %s succeeded", numStr);
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
    free(workMatrix);
    free(workConstants);
    free(answers);
    free(numStr);

    return NOMINAL_RUN;
}