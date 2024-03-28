/*
 -------------------------------------
 File:    process_management.c
 Project: a02
 Operating Systems: Q2
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
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_COMMAND_LENGTH 256

void writeOutput(char* command, char* output) {
    FILE* fp = fopen("output.txt", "a");
    if (fp == NULL) {
        printf("Error: cannot open output file");
        exit(1);
    }

    fprintf(fp, "The output of: %s : is\n", command);
    fprintf(fp, ">>>>>>>>>>>>>>>\n%s<<<<<<<<<<<<<<<\n", output);

    fclose(fp);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    const char* input_file = argv[1];
    int pipe_fd[2];
    pid_t child_pid;

    if (pipe(pipe_fd) == -1) {
        printf("Error: pipe creation failed");
        return 1;
    }

    child_pid = fork();

    if (child_pid == -1) {
       printf("Error: fork failed");
        return 1;
    }

    if (child_pid == 0) {
        // Child process reads input file and writes to shared memory
        int shm_fd;
        char* shared_memory;
        shm_fd = shm_open("/shared_memory", O_CREAT | O_RDWR, 0666);
        ftruncate(shm_fd, MAX_COMMAND_LENGTH);
        shared_memory = (char*)mmap(0, MAX_COMMAND_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

        FILE* file = fopen(input_file, "r");
        if (file == NULL) {
            printf("Error: cannot open input file");
            exit(1);
        }

        char command[MAX_COMMAND_LENGTH];
        char file_content[MAX_COMMAND_LENGTH] = "";
        while (fgets(command, sizeof(command), file) != NULL) {
            strcat(file_content, command);
        }

        strcpy(shared_memory, file_content);

        fclose(file);
        close(shm_fd);
        munmap(shared_memory, MAX_COMMAND_LENGTH);

        exit(0);
    } else {
        // Parent process reads from shared memory
        wait(NULL);
        char* shared_memory;
        int shm_fd = shm_open("/shared_memory", O_RDWR, 0);
        shared_memory = (char*)mmap(0, MAX_COMMAND_LENGTH, PROT_READ, MAP_SHARED, shm_fd, 0);

        char commands[MAX_COMMAND_LENGTH];
        strcpy(commands, shared_memory);
        close(shm_fd);
        munmap(shared_memory, MAX_COMMAND_LENGTH);

        // Tokenize the commands
        char* token = strtok(commands, "\n");
        while (token != NULL) {
            int child_pipe_fd[2];
            if (pipe(child_pipe_fd) == -1) {
                printf("Error: child pipe creation failed");
                return 1;
            }

            child_pid = fork();

            if (child_pid == -1) {
                printf("Error: child fork failed");
                return 1;
            }

            if (child_pid == 0) {
                // Child process executes the command and writes to the pipe
                close(child_pipe_fd[0]);
                dup2(child_pipe_fd[1], STDOUT_FILENO);
                char* args[] = {"/bin/sh", "-c", token, NULL};
                execvp(args[0], args);
                perror("Exec failed");
                exit(1);
            } else {
                // Parent process reads from the pipe and writes to "output.txt"
                close(child_pipe_fd[1]);
                char output[MAX_COMMAND_LENGTH];
                int nbytes = read(child_pipe_fd[0], output, sizeof(output));
                output[nbytes] = '\0';
                close(child_pipe_fd[0]);
                writeOutput(token, output);
                wait(NULL);
            }

            token = strtok(NULL, "\n");
        }
    }

    return 0;
}
