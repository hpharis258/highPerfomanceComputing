/*************************************************************
 * Question 1: Password Cracking Using multithreading.
 * Instructions: Input Encrypted password and the number of
 * threads that you want to use.
 * By: Haroldas Varanauskas
 * Student Number: 2411253
 *************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <crypt.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

// Struct used to Split Work for each thread.
struct SingleThreadWork
{
    int start;
    int end;
    char *pass;
};
// Is Password Found
bool isPasswordFound = false;
// Global Mutex
pthread_mutex_t mutex;
// Substring Function
void substr(char *dest, char *src, int start, int length){
  memcpy(dest, src + start, length);
  *(dest + length) = '\0';
}
//
void *crackPassword(void *p){
  // Get passed in struct
  struct SingleThreadWork *passedStruct = p;
  int start = passedStruct->start + 65;
  int end = passedStruct->end + 65;
  char *passThread = passedStruct->pass;
  //Salt, Plain and ecrypted.
  char salt[7];
  char plain[7];
  char *encrypted;
  int x, y, z;// Loop counters
  //
  substr(salt, passThread, 0, 6);
  for(x=start; x<=end; x++){
    for(y='A'; y<='Z'; y++){
      for(z=0; z<=99; z++){
        if(isPasswordFound)
        {
            // PASSWORD FOUND exit thread
            pthread_exit(NULL);
        }
        // Format String
        sprintf(plain, "%c%c%02d", x, y, z);
        // encrypt generated password
        pthread_mutex_lock(&mutex);
        encrypted = (char *) crypt(plain, salt); // I have locked this because it doesn't work otherwise, i think the threads either share or access at the same time the string pointer returned by crypt function.
        pthread_mutex_unlock(&mutex);
        // Check if Password Found
        if(strcmp(passThread, encrypted) == 0){
        //PASSWORD FOUND
        printf("Password Found: %s, Encrypted Password:  %s\n",plain, encrypted);
        isPasswordFound = true;
        pthread_exit(NULL);
        }else{
        // Password not found
         //printf("Combinations Explored: %-8d, Password Not Found: %s, Encrypted Password:  %s\n", Explored_combinations, plain, encryted); //when the password is not found
        }
       
      }
      
     
    }
  }
 
}

// Determine the amount of work for each thread
void determineThreadWorkLoad(int ComputationAmountInput, int threadCount, struct SingleThreadWork threadWorkArray[])
{
    long compCount = (long) ComputationAmountInput;
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
    struct SingleThreadWork returnArray[threadCount];
   
   
    for (int k = 0; k < threadCount; k++){
        if(k == 0){
            startList[k] = 0;
            endList[k] = startList[k] + sliceList[k] - 1;
        }else{
            startList[k] = endList[k-1] + 1;
            endList[k] = startList[k] + sliceList[k] - 1;
        }
    }
   
    for (int g = 0; g < threadCount; g++){
        struct SingleThreadWork tempStruct = {startList[g], endList[g]};
        threadWorkArray[g] = tempStruct;
        //printf("Thread: %d, start = %d  end = %d\n", g ,startList[g], endList[g]);
    }
}



int main(int argc, char* argv[])
{
    // Init thread number and password input.
    int numberOfThreads;
    char PasswordHash[92];
    printf("Hello and Welcome to your password cracker!\n");
    printf("Please insert the hash of the password you want to crack!\n");
    // Take input of the password hash;
    scanf("%s", &PasswordHash);
    printf("Password Hash Entered: %s\n", PasswordHash);
    printf("Enter the number of threads you want to use: ");
    scanf("%d", &numberOfThreads);
    printf("You have Entered: %d\n", numberOfThreads);
    // Init Work For each thread Array
    struct SingleThreadWork workForEachThread[numberOfThreads];
    // init mutex.
    pthread_mutex_init(&mutex, NULL);
    // init Number of Threads
    pthread_t threads[numberOfThreads];
    // split work for each thread.
    determineThreadWorkLoad(26, numberOfThreads, workForEachThread);
    // Create thread for number of threads required
    for(int i = 0; i < numberOfThreads; i++)
    {
        // CREATE THREADS
        workForEachThread[i].pass = PasswordHash;
        pthread_create(&threads[i], NULL, crackPassword, &workForEachThread[i]);
        printf("Thread id %d, Start: %d, End: %d\n", i, workForEachThread[i].start, workForEachThread[i].end );
    }
    // JOIN THREADS
    for(int i = 0; i < numberOfThreads; i++)
    {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
}
// TEST PASSWORD USED HP93
// TEST HASH USED
// $6$AS$Ig.vW9RG9J5gPFUvHwyV67GdVVndF.2ROH6.qZjQN1Nm5kqn0t/FKNf4.48qRHdyAWwIQOtKkCosTrwyj3SvJ.
