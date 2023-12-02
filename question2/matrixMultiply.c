#include <stdio.h>
#include <stdlib.h>

void main(int argc, char** argv)
{
    FILE *fp = NULL;
    int row, col;
    int rows, cols;
    float matval= 0.0;
    //
    fp = fopen("MatData.txt", "r");
    fscanf(fp, "%d %d", &cols, &rows);
    for(row = 0; row < rows; rows++)
    {6f
        printf("[");
        for(col = 0; col < cols; col++)
        {
            fscanf(fp, "%f", &matval);
            printf("%f \t", &matval);

        }
        //fscanf(fp, )
    }
}