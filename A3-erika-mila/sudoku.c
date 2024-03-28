/*
 -------------------------------------
 File:    sudoku.c
 Project: a03
 Operating Systems: Q1
 -------------------------------------
 Author:  Mila Cvetanovska, Erika Capper
 ID:      210311400, 210349750
 Email:   cvet1400@mylaurier.ca, capp9750@mylaurier.ca
 Version  2023-10-25
 -------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 9

// Structure to pass data to threads
typedef struct {
    int row;
    int col;
} Parameters;

int sudoku[N][N];
int results[N + N + N]; // Array to store thread results

// Function to check if a row contains all digits 1-9
void *checkRow(void *param) {
    Parameters *data = (Parameters *)param;
    int row = data->row;
    int isValid = 1; // Assume the row is valid

    int check[N] = {0};
    for (int col = 0; col < N; col++) {
        int digit = sudoku[row][col];
        if (check[digit - 1] == 1) {
            isValid = 0; // Row is invalid
            break;
        }
        check[digit - 1] = 1;
    }
    
    results[row] = isValid;
    pthread_exit(NULL);
}

// Function to check if a column contains all digits 1-9
void *checkColumn(void *param) {
    Parameters *data = (Parameters *)param;
    int col = data->col;
    int isValid = 1; // Assume the column is valid

    int check[N] = {0};
    for (int row = 0; row < N; row++) {
        int digit = sudoku[row][col];
        if (check[digit - 1] == 1) {
            isValid = 0; // Column is invalid
            break;
        }
        check[digit - 1] = 1;
    }

    results[N + col] = isValid;
    pthread_exit(NULL);
}

// Function to check if a 3x3 subgrid contains all digits 1-9
void *checkSubgrid(void *param) {
    Parameters *data = (Parameters *)param;
    int row = data->row;
    int col = data->col;
    int isValid = 1; // Assume the subgrid is valid

    int check[N] = {0};
    for (int i = row; i < row + 3; i++) {
        for (int j = col; j < col + 3; j++) {
            int digit = sudoku[i][j];
            if (check[digit - 1] == 1) {
                isValid = 0; // Subgrid is invalid
                break;
            }
            check[digit - 1] = 1;
        }
    }

    results[N + N + (row / 3) * 3 + (col / 3)] = isValid;
    pthread_exit(NULL);
}

int main() {
    FILE *file = fopen("sample_in_sudoku.txt", "r");
    if (file == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fscanf(file, "%d", &sudoku[i][j]);
        }
    }
    fclose(file);

    pthread_t threads[N + N + N];
    Parameters params[N + N + N];

    // Create threads to check rows
    for (int i = 0; i < N; i++) {
        params[i].row = i;
        pthread_create(&threads[i], NULL, checkRow, &params[i]);
    }

    // Create threads to check columns
    for (int i = N; i < N + N; i++) {
        params[i].col = i - N;
        pthread_create(&threads[i], NULL, checkColumn, &params[i]);
    }

    // Create threads to check 3x3 subgrids
    int threadIndex = N + N;
    for (int i = 0; i < N; i += 3) {
        for (int j = 0; j < N; j += 3) {
            params[threadIndex].row = i;
            params[threadIndex].col = j;
            pthread_create(&threads[threadIndex], NULL, checkSubgrid, &params[threadIndex]);
            threadIndex++;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < N + N + N; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Sudoku Puzzle Solution is:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d", sudoku[i][j]);
            if (j < N - 1) {
                printf("   ");
            }
        }
        printf("\n");
    }

    // Check results
    int isValid = 1;
    for (int i = 0; i < N + N + N; i++) {
        if (results[i] == 0) {
            isValid = 0; // Puzzle is invalid
            break;
        }
    }

    if (isValid) {
        printf("Sudoku puzzle is valid.\n");
    } else {
        printf("Sudoku puzzle is invalid.\n");
    }

    return 0;
}