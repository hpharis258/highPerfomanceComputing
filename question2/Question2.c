/*************************************************************
 * Question 2: Matrix Multiplication using  multithreading.
 * Instructions: First Argument is the file name.
 * second is the number of threads you want to use.
 * By: Haroldas Varanauskas
 * Student Number: 2411253
 *************************************************************/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

// STRUCT STORES MATRIX, row and col.
struct Matrix{
    int Matrix_ROWS;
    int Matrix_COLS;
    double **MATRIX;
};

struct Single_thread_work{
    // Thread Work
    int start;
    int end;
    struct Matrix Matrix_A;
    struct Matrix Matrix_B;
    struct Matrix ResultingMatrix;
};

void * multiplyMatrix(void *p);
void determineThreadWorkLoad(int ComputationAmount, int threadCount, int *returnStartList, int *returnEndList);

int main(int argc, char *argv[]){
    printf("Welcome to Matrix Multiplier!\n");
    printf("Number of arguments passed in %d\n", argc - 1);
    if(argc != 3)
    {
        printf("Please pass in 2 arguments and retry!\n");
        return 0;
    }
    //
    FILE *filePointer;
    char *inputFileName = argv[1];
    int numberOfThreads = atoi(argv[2]);
    printf("Passed in File Name: %s\n", inputFileName);
    printf("Passed in Number of Threads: %d\n", numberOfThreads);
    // Open the file
    filePointer = fopen(inputFileName, "r");
    
    char buff[200];
    char buffer[255];
   
     // Current Matrix
    int rows_matrix_current = 0;
    int cols_matrix_current = 0;
    int temp_row = 0;
    int temp_col = 0;
    //
    int matrixCount = 0;
    int matrC = 1;
    //
    int addedMatrixCount = 0;
    int row_counter = 0;
    double currentNumber;
    // Readding
    int adding_row =0;
    int adding_col =0;
    char *line, *number;
    //
    if(filePointer != NULL)
    {
        // Get Number of Matrices in the File
        while(fscanf(filePointer, "%s", buff) == 1)
        {
            if(sscanf(buff, "%d,%d", &rows_matrix_current, &cols_matrix_current) == 2)
            {
            matrixCount++;
            }
        }
        printf("Number of Matrices found: %d\n", matrixCount);
        // Init array of Matrices with the number found above;
        struct Matrix matricesArray[matrixCount];
        // Reset File pointer
        rewind(filePointer);
        //
        double **currentMatrix;
        while(fgets(buffer, 255, filePointer) != NULL)
        {
            // Read in First Line and Allocate Current Matrix
            if(sscanf(buffer, "%d,%d", &rows_matrix_current, &cols_matrix_current) == 2)
            {
                temp_row = rows_matrix_current;
                temp_col = cols_matrix_current;
                //printf("Matrix %d ROWS: %d COLS: %d\n", matrC,temp_row, temp_col);
                matrC++;
                // Allocate Matrix rows
                currentMatrix = (double **)malloc(temp_row * sizeof(double *));
                for(int k = 0; k < temp_row; k++)
                {
                    // Allocate Matrix Columns
                    currentMatrix[k] = (double *)malloc(temp_col * sizeof(double *));
                }
                // reset
                adding_col = 0;
                adding_row = 0;
            }
            else
            {
                //printf("%s\n", buffer);
                // Split each line
                adding_col = 0;
                number = strtok(buffer, ",");
                while(number != NULL)
                {
                    currentNumber = atof(number);
                    if(currentNumber != 0.000000)
                    {
                        //printf("%lf\n", currentNumber);
                        //printf("%s\n", number);
                        currentMatrix[adding_row][adding_col] = currentNumber;
                        adding_col++;
                    }
                    
                    number = strtok(NULL, ",");
                }
                adding_row++;
                // Add Matrix To Array
                if(adding_row == temp_row)
                {
                    struct Matrix currMat;
                    currMat.Matrix_ROWS = temp_row;
                    currMat.Matrix_COLS = temp_col;
                    currMat.MATRIX = currentMatrix;
                    matricesArray[addedMatrixCount] = currMat;
                    addedMatrixCount++;
                }

            }
        }
        //Check matrices that are in memory.
        
     //   for(int i =0; i < addedMatrixCount; i++)
     //   {
     //       double **Mat = matricesArray[i].MATRIX;
     //       int row = matricesArray[i].Matrix_ROWS;
     //       int col = matricesArray[i].Matrix_COLS;
     //       for(int g = 0; g < row; g++)
     //       {
     //           for(int h = 0; h < col; h++)
     //           {
     //               printf("%lf", Mat[g][h]);
     //           }
     //           printf("\n");
     //
     //       }
     //       printf("\n");
     //   }
     
     //
     // Check
     if(addedMatrixCount % 2 != 0)
     {
         printf("Matrix count not even! add even number of matrices and try again.\n");
         return 1;
     }
     // Init Work Array
     struct Single_thread_work workArray[addedMatrixCount / 2];
     // Add Matrices to Work Array.
     int tempRows;
     int tempCols;
     int count = 0;
     for(int i = 0; i < addedMatrixCount; i = i + 2)
     {
         struct Single_thread_work work;
         work.Matrix_A = matricesArray[i];
         work.Matrix_B = matricesArray[i+1];
         // Calculate Resulting Matrix Rows And Colums for the resulting matrix
         tempRows = work.Matrix_A.Matrix_ROWS;
         tempCols = work.Matrix_B.Matrix_COLS;
         // , Matrix A Cols Must be = to Matrix B rows
         // Resulting Matrix = Matrix A Rows, Matrix B Columns
         work.ResultingMatrix.Matrix_ROWS = tempRows;
         work.ResultingMatrix.Matrix_COLS = tempCols;
         if(tempRows != tempCols)
         {
             printf("Can't Multiply because Matrix A Rows: %d and Matrix B Columns: %d\n",tempRows, tempCols);
         }else
         {
             // Allocate Memory for the Matrix
             work.ResultingMatrix.MATRIX = (double **)malloc(tempRows * sizeof(double *));
             for(int k = 0; k < tempRows; k++)
                {
                    // Allocate Matrix Columns
                    work.ResultingMatrix.MATRIX[k] = (double *)malloc(tempCols * sizeof(double *));
                }
             // Add Matrix To Array
             workArray[count] = work;
             count++;
         }
         
     }
     int resultingMatrixCount = addedMatrixCount / 2;
     int passedInThreadNum = numberOfThreads;
     // Determine Work For Each Thread
     for(int i = 0; i < addedMatrixCount / 2; i++)
     {
         //printf("Dividing thread work for Matrix Pair %d \n", i + 1);
         // Limit Num of Threads
         if(numberOfThreads > workArray[i].ResultingMatrix.Matrix_ROWS)
         {
             passedInThreadNum = workArray[i].ResultingMatrix.Matrix_ROWS;
         }
         else
         {
             passedInThreadNum = numberOfThreads;
         }
         // THREADS Array
         pthread_t someThreads[passedInThreadNum];
         int thread_start[passedInThreadNum];
         int thread_end[passedInThreadNum];
         // POPULATE THREAD START AND END ARRAYS
         determineThreadWorkLoad(workArray[i].ResultingMatrix.Matrix_ROWS, passedInThreadNum, thread_start, thread_end);
         // Work For Each Thread
         struct Single_thread_work workForEachThreadArray[passedInThreadNum];
         
         
         //
         for(int th = 0; th < passedInThreadNum; th++)
         {
             // Copy the Current Struct to work For Each Thread and determine start/end for that thread
             memcpy(&workForEachThreadArray[th], &workArray[i], sizeof(struct Single_thread_work));
             workForEachThreadArray[th].start = thread_start[th];
             workForEachThreadArray[th].end = thread_end[th];
             // DETERMINE WORK FOR CURRENT THREAD
             
             //printf("Thread start: %d, end: %d in struct\n", workForEachThreadArray[th].start, workForEachThreadArray[th].end);
         }
         // Create Threads
         for(int the = 0; the < passedInThreadNum; the++)
         {
             
             //printf("THREAD NUMBER: %d has passed in start %d, end %d\n", the,workForEachThreadArray[the].start, workForEachThreadArray[the].end);
             pthread_create(&someThreads[the], NULL, multiplyMatrix, &workForEachThreadArray[the]);
             
         }
         // JOIN
         for(int e =0; e < passedInThreadNum; e++)
         {
             pthread_join(someThreads[e], NULL);
         }
         
         //multiplyMatrix(workArray[i]);
     }
     double checkNum;
     // Check Multiplied Matrices PRINT
     //for(int mt = 0; mt < resultingMatrixCount; mt++)
     //{
         //printf("\n");
    //     printf("Matrix Rows %d, Cols %d\n", workArray[mt].ResultingMatrix.Matrix_ROWS, workArray[mt].ResultingMatrix.Matrix_COLS);
     //    for(int mt1 = 0; mt1 < workArray[mt].ResultingMatrix.Matrix_ROWS; mt1++)
     //    {
     //        for(int mt2 = 0; mt2 <  workArray[mt].ResultingMatrix.Matrix_COLS; mt2++)
     //        {
     //             checkNum = workArray[mt].ResultingMatrix.MATRIX[mt1][mt2];
     //            printf("%lf,", checkNum);
     //         }
     //        printf("\n");
    //     }
    // }
     // Write Multiplied Matrices to a file;
     FILE *outputFile;
     outputFile = fopen("outputFile.txt", "w");
     for(int h = 0; h < resultingMatrixCount; h++)
     {
         fprintf(outputFile, "Matrix Rows %d, Cols %d\n", workArray[h].ResultingMatrix.Matrix_ROWS, workArray[h].ResultingMatrix.Matrix_COLS);
         for(int h1 = 0; h1 < workArray[h].ResultingMatrix.Matrix_ROWS; h1++)
         {
             for(int h2 = 0; h2 < workArray[h].ResultingMatrix.Matrix_COLS; h2++)
             {
                 checkNum = workArray[h].ResultingMatrix.MATRIX[h1][h2];
                if(h2 == workArray[h].ResultingMatrix.Matrix_COLS - 1)
                {
                    // Last Col
                    fprintf(outputFile, "%lf", checkNum);
                }else
                {
                    fprintf(outputFile, "%lf,", checkNum);
                }
                 
             }
             fprintf(outputFile, "\n");
         }
         fprintf(outputFile, "\n");
     }
     
     
    }
    else
    {
        printf("An Error occured when trying to open the file!\n");
        return 1;
    }
    
    // Thread Stuff
    return 0;
}

void * multiplyMatrix(void *p)
{
    struct Single_thread_work *passedStruct = p;
    struct Matrix Matrix_A = passedStruct->Matrix_A;
    struct Matrix Matrix_B = passedStruct->Matrix_B;
    struct Matrix ResultMatrix = passedStruct->ResultingMatrix;
    // Row Start and End
    int start = passedStruct->start;
    int end = passedStruct->end;
    //printf("I have Passed in START: %d and END %d\n", start, end);
    
    
        // Multiply
        //printf("Multiplying Matrices: Matrix A Rows: %d Matrix B Columns %d\n", Matrix_A.Matrix_ROWS, Matrix_B.Matrix_COLS);
        
            // Multiply the Matrix;
            // Fill Resulting Matrix Rows From Start to Finish
            // ROWS of Resulting matrix
            for(int i = start; i < end + 1; i++)
            {
                // Cols of resulting matrix
                for(int j = 0; j < ResultMatrix.Matrix_COLS; j++)
                {
                    ResultMatrix.MATRIX[i][j] = 0.0;
                    for(int k = 0; k < Matrix_B.Matrix_ROWS; k++)
                    {
                        ResultMatrix.MATRIX[i][j] += Matrix_A.MATRIX[i][k] *Matrix_B.MATRIX[k][j];
                    }
                }


                //printf("Computing Row %d of resulting matrix \n", i);
                    //
                //for(int j = 0; j < ResultMatrix.Matrix_COLS; j++)
                //{
                    // Init to 0
                //    ResultMatrix.MATRIX[i][j] = 0.0;
                //    for(int q = 0; q < Matrix_A.Matrix_COLS; q++)
                //    {
                //        ResultMatrix.MATRIX[i][j] += Matrix_A.MATRIX[i][q] + Matrix_B.MATRIX[q][j];
                //    }
                //}
            }
    
    
}

// Comp Ammount = Number of Rows in Resulting Matrix,
void determineThreadWorkLoad(int ComputationAmount, int threadCount, int *returnStartList, int *returnEndList){
     long compCount = (long) ComputationAmount;
        //
         int sliceList[threadCount];
        int remainder = compCount % threadCount;
       
        //fill standard slices (without remainder)
        for (int i = 0; i < threadCount; i++){
        sliceList[i] = compCount / threadCount;
        }
       
        //take remainder and add extra computation
        for (int j = 0; j < remainder; j++){
        sliceList[j] = sliceList[j] + 1;
        }
       
        int startList[threadCount];
        int endList[threadCount];
        //struct SingleThreadWork returnArray[threadCount];
       
       
        for (int k = 0; k < threadCount; k++){
        if(k == 0){
            startList[k] = 0;
            endList[k] = startList[k] + sliceList[k] - 1;
        }else{
            startList[k] = endList[k-1] + 1;
            endList[k] = startList[k] + sliceList[k] - 1;
        }
        }
       
        for(int g = 0; g < threadCount; g++)
        {
            returnStartList[g] = startList[g];
            returnEndList[g] = endList[g];
        }
        
}

