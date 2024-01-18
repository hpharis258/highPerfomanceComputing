/*************************************************************
 * Question 3: Password Cracking using CUDA.                                         
 * By: Haroldas Varanauskas
 * Student Number: 2411253
 *************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Cuda Crypt Function
__device__
char* CudaCrypt(char* rawPassword) {
	
	// Allocate memory for password
	char * newPassword = (char *) malloc(sizeof(char) * 11);

	newPassword[0] = rawPassword[0] + 2;
	newPassword[1] = rawPassword[0] - 2;
	newPassword[2] = rawPassword[0] + 1;
	newPassword[3] = rawPassword[1] + 3;
	newPassword[4] = rawPassword[1] - 3;
	newPassword[5] = rawPassword[1] - 1;
	newPassword[6] = rawPassword[2] + 2;
	newPassword[7] = rawPassword[2] - 2;
	newPassword[8] = rawPassword[3] + 4;
	newPassword[9] = rawPassword[3] - 4;
	newPassword[10] = '\0';

	for(int i =0; i<10; i++){
		if(i >= 0 && i < 6){ //checking all lower case letter limits
			if(newPassword[i] > 122){
				newPassword[i] = (newPassword[i] - 122) + 97;
			}else if(newPassword[i] < 97){
				newPassword[i] = (97 - newPassword[i]) + 97;
			}
		}else{ //checking numbers
			if(newPassword[i] > 57){
				newPassword[i] = (newPassword[i] - 57) + 48;
			}else if(newPassword[i] < 48){
				newPassword[i] = (48 - newPassword[i]) + 48;
			}
		}
	}
	return newPassword;
}

// Function to check if two strings are the same.
__device__ 
int isStringMatching(char* StringOne, char* StringTwo, int length) {
	int result = 1;
	for (int i = 0; i < length; i++) {
		if (StringOne[i] != StringTwo[i]) {
			result = 0;
			break;
		}
	}
	return result;
}

// DECRYPT PASSWORD FUNCTION
__global__
void decryptPassword(char* alphabet, char* numbers, char* encryptedPass, char* deviceOutputPass)
{
	/// Get cuda unique thread id
	int uniqueid = blockDim.x * blockIdx.x + threadIdx.x;
	
	/// Check if another thread found output pass before starting
	if(*deviceOutputPass != NULL) {
		// Early Return Another Thread Found Password.
		return;
	}

	/// Create pass to check on this thread
	char GeneratedPass[4];
	GeneratedPass[0] = alphabet[blockIdx.x];
	GeneratedPass[1] = alphabet[blockIdx.y];
	GeneratedPass[2] = numbers[threadIdx.x];
	GeneratedPass[3] = numbers[threadIdx.y];
	
	/// Encrypt the potential password
	char* encryptedPotentialPassword;
	encryptedPotentialPassword = CudaCrypt(GeneratedPass);
	
	/// check if encrypted password string is the same as the encrypted potential password string.
	if (isStringMatching(encryptedPass, encryptedPotentialPassword, 11) > 0 )
	{
		/// Password strings are the same so set deviceOutputPassword to the current generated password
		printf("THREAD ID '%d' Encrypted password '%s' : '%s' matches password = '%s'\n", uniqueid, encryptedPass, GeneratedPass, encryptedPotentialPassword);
		// Loop to copy the string
		for (int i = 0; i < 4; i++ ) {
			deviceOutputPass[i] = GeneratedPass[i];
		}
	}
}

/**
	
*/
int main(int argc, char** argv) {
	printf("Welcome to the CUDA password cracker!!!\n"); 
	//Test Encrypted Passwords:
	//aa52 = ccbddb7362
	char* encryptedPass;
	// pass in the encrypted password as a command line argument if you want to. 
	if (argc > 1) {
		encryptedPass = argv[1];
	}else{
	 encryptedPass = "ccbddb7362";
	}
	 
	printf("Encrypted Password used: '%s'\n", encryptedPass);
	
	// Init abc and numbers array to read only use in cuda 
	char cpuABC[26] = { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z' };
	char cpuNumbers[10] = { '0', '1', '2', '3', '4', '5', '6' ,'7', '8', '9' };
	
	int sizeOfEncryptedPass = sizeof(char) * 11;
	
	char* gpuABC;
	cudaMalloc( (void**) &gpuABC, sizeof(char) * 26 );
	cudaMemcpy( gpuABC, cpuABC, sizeof(char) * 26, cudaMemcpyHostToDevice );
	
	char* gpuNumbers;
	cudaMalloc( (void**) &gpuNumbers, sizeof(char) * 10 );
	cudaMemcpy( gpuNumbers, cpuNumbers, sizeof(char) * 10, cudaMemcpyHostToDevice );
	
	char* gpuEncryptedPass;
	cudaMalloc( (void**) &gpuEncryptedPass, sizeOfEncryptedPass );
	cudaMemcpy( gpuEncryptedPass, encryptedPass, sizeOfEncryptedPass, cudaMemcpyHostToDevice);
	
	char* gpuOutputPass;
	cudaMalloc( (void**) &gpuOutputPass, sizeOfEncryptedPass );
	
	// 26 block for x, 26 block for y, Blocks divide the letters, threads divide the numbers
	decryptPassword<<< dim3(26, 26, 1), dim3(10, 10, 1) >>>(gpuABC, gpuNumbers, gpuEncryptedPass, gpuOutputPass);
	// Synchronize threads
	cudaDeviceSynchronize();
	// Allocate Memory for the Output password.
	char* cpuOutputPass = (char*)malloc( sizeof(char) * 4 );
	/// Copy GPU output pass to the CPU : Device to Host
	cudaMemcpy(cpuOutputPass, gpuOutputPass, sizeOfEncryptedPass, cudaMemcpyDeviceToHost);
	/// If output password is not null or empty, print output password
	if (cpuOutputPass != NULL && cpuOutputPass[0] != 0) {
		printf("Given Encrypted Password: '%s'\n", encryptedPass);
		printf("Found Decrypted Password: '%s'\n", cpuOutputPass);
	} else {
		printf("Can't find a password.\n");
	}
	/// Free all of the used Memory
	cudaFree(gpuABC);
	cudaFree(gpuNumbers);
	cudaFree(gpuEncryptedPass);
	cudaFree(gpuOutputPass);
	free(cpuOutputPass);
	return 0;
}