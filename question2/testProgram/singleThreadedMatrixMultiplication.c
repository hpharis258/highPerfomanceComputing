#include <stdlib.h>
#include <stdio.h>

// MATRIX dimensions
#define R1 2 // NUMBER OF ROWS in Matrix 1
#define C1 2 // Number of Columns in Matrix 1
#define R2 4 // Number of Rows in MATRIX 2
#define C2 3 // Number of Columns in MATRIX 2

void multiplyMatrix(int m1[][C1], int m2[][C2])
{
    int result[R1][C2];
    printf("Resultant Matrix is: \n");
    //
    for(int i = 0; i < R1; i++)
    {
        for(int j = 0; j < C2; j++)
        {
            result[i][j] = 0;
            for(int k = 0; k < R2; k++)
            {
                result[i][j] += m1[i][k] * m2[k][j];
            }
            printf("%d\t", result[i][j]);
        }
        printf("\n");
    }
}

//
int main()
{
    int m1[R1][C1] = {{1, 1} , {2, 2}};
    int m2[R2][C2] = {{1,1,1}, {2,2,2}};
    // CHECK IF MATRIX 1 COLUMNS are EQUAL TO ROWS of MATRIX 2
    if(C1 != R2)
    {
        printf("The Number of COLUMNS in MATRIX 1 needs to be equal to number of ROWS in MATRIX 2\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        multiplyMatrix(m1, m2);
    }
    return 0;
}


// USE PTHREADS POSIX THREADS 1995 STANDARD // RTFM ->
// USE SLICING.C
// Pull Data From structs
// 