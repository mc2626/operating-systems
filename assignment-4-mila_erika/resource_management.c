/*
 -------------------------------------
 File:    resource_management.c
 Project: a04
 Operating Systems: Q1
 -------------------------------------
 Author:  Mila Cvetanovska, Erika Capper
 ID:      210311400, 210349750
 Email:   cvet1400@mylaurier.ca, capp9750@mylaurier.ca
 Version  2023-11-07
 -------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_RESOURCES 5
int available_resources = MAX_RESOURCES;
#define NUM_THREADS 5

// To protect critical sections, mutex lock is created
pthread_mutex_t mmutex = PTHREAD_MUTEX_INITIALIZER;

/* decrease available_resources by count resources */
/* return 0 if sufficient resources available, */
/* otherwise return -1 */
int decrease_count(int thread_number, int count) {
    pthread_mutex_lock(&mmutex); // Critical section is locked 

    if (available_resources < count) {
        pthread_mutex_unlock(&mmutex); // Lock is released
        return -1;
    } else {
        available_resources -= count;
        printf("The thread %d has acquired, %d resources and %d more resources are available.\n",
               thread_number, count, available_resources);
        pthread_mutex_unlock(&mmutex); // Lock is released
        return 0;
    }
}

/* increase available_resources by count */
int increase_count(int thread_number, int count) {
    pthread_mutex_lock(&mmutex); // Critical section is locked 

    available_resources += count;
    printf("The thread %d has released, %d resources and %d resources are now available.\n",
           thread_number, count, available_resources);

    pthread_mutex_unlock(&mmutex); // Lock is released
    return 0;
}

/* Function to determine if thread can successfully obtain a resource*/
void* thread_function(void* thread_number) {
    int thread_num = *((int*)thread_number);

    if (decrease_count(thread_num, 1) == 0) {
        sleep(1);
        increase_count(thread_num, 1);
    } else {
        printf("Thread %d could not acquire enough resources.\n", thread_num);
    }

    pthread_exit(NULL);
}

/* Driver Code */
int main() {
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];

    pthread_mutex_init(&mmutex, NULL);

    // Create threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_args[i] = i;
        if (pthread_create(&threads[i], NULL, thread_function, &thread_args[i]) != 0) {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Error joining thread");
            exit(EXIT_FAILURE);
        }
    }

    // Print the final available resources
    printf("All threads have finished execution. Available resources: %d\n", available_resources);

    // Destroy the mutex
    pthread_mutex_destroy(&mmutex);

    return 0;
}
