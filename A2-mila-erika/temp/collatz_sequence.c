/*
 -------------------------------------
 File:    collatz_sequence.c
 Project: a02
 Operating Systems: Q1
 -------------------------------------
 Author:  Mila Cvetanovska, Erika Capper
 ID:      210311400, 210349750
 Email:   cvet1400@mylaurier.ca, capp9750@mylaurier.ca
 Version  2023-10-12
 -------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_NUMBERS 10
#define MAX_SEQUENCE 100

void make_collatz_sequence(int number, int* seq, int* seq_length) {
    // Generate Collatz sequence for the given number
    int count = 0;
    int num = number;
    while (num != 1 && count < MAX_SEQUENCE) {
        seq[count] = num;
        if (num % 2 == 0)
            num = num / 2;
        else
            num = 3 * num + 1;
        count++;
    }
    seq[count] = 1;
    *seq_length = count + 1;
}

// ----------------------- TESTING ---------------------------

int main() {
    // Read start numbers from a file
    int startNumbers[MAX_NUMBERS];
    int numNumbers = 0;

    FILE* file = fopen("start_numbers.txt", "r");
    if (file == NULL) {
        printf("Error: Cannot open file");
        exit(1);
    }

    while (fscanf(file, "%d", &startNumbers[numNumbers]) != EOF) {
        numNumbers++;
    }
    fclose(file);

    // Shared memory setup
    int shm_fd;
    int* shared_memory;
    shm_fd = shm_open("/collatz_sequence", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, MAX_SEQUENCE * sizeof(int));
    shared_memory = (int*)mmap(0, MAX_SEQUENCE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    for (int i = 0; i < numNumbers; i++) {
        int sequence[MAX_SEQUENCE];
        int seqLength;

        make_collatz_sequence(startNumbers[i], sequence, &seqLength);

        // Copy the sequence to shared memory
        for (int j = 0; j < seqLength; j++) {
            shared_memory[j] = sequence[j];
        }

        // Fork a child process
        pid_t pid = fork();

        if (pid < 0) {
            printf("Error: fork failed");
            exit(1);
        } else if (pid == 0) { // Child process
            printf("Parent Process: The positive integer read from the file is %d \n", startNumbers[i]);
            printf("Child Process: The generated collatz sequence is ");
            for (int j = 0; j < seqLength; j++) {
                printf("%d ", shared_memory[j]);
            }
            printf("\n");
            shm_unlink("/collatz_sequence"); // Remove shared memory object
            exit(0);
        } else { // Parent process
            wait(NULL); // Wait for the child process to finish
        }
    }

    // Cleanup
    munmap(shared_memory, MAX_SEQUENCE * sizeof(int));
    close(shm_fd);
    shm_unlink("/collatz_sequence");

    return 0;
}
