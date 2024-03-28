/*
 -------------------------------------
 File:    fcfs.c
 Project: a03
 Operating Systems: Q2
 -------------------------------------
 Author:  Mila Cvetanovska, Erika Capper
 ID:      210311400, 210349750
 Email:   cvet1400@mylaurier.ca, capp9750@mylaurier.ca
 Version  2023-10-25
 -------------------------------------
 */

#include <stdio.h>

struct threadInfo {
    int p_id;
    int arr_time;
    int burst_time;
    int waiting_time;
    int turn_around_time;
};

int main() {
    FILE *file = fopen("sample_in_schedule.txt", "r");
    if (file == NULL) {
        printf("Error: File not found/cannot be opened.\n");
        return 1;
    }

    printf("Thread ID   Arrival Time   Burst Time   Completion Time   Turn-Around Time   Waiting Time\n");

    struct threadInfo threads[100]; // Assuming a maximum of 100 threads
    int num_threads = 0;
    int current_time = 0;

    while (fscanf(file, "%d, %d, %d", &threads[num_threads].p_id, &threads[num_threads].arr_time, &threads[num_threads].burst_time) != EOF) {
        num_threads++;
    }

    fclose(file);

    // Schedule the threads using FCFS
    for (int i = 0; i < num_threads; i++) {
        if (current_time < threads[i].arr_time) {
            current_time = threads[i].arr_time;
        }
        threads[i].turn_around_time = current_time + threads[i].burst_time - threads[i].arr_time;
        threads[i].waiting_time = threads[i].turn_around_time - threads[i].burst_time;
        current_time += threads[i].burst_time;

        printf("%d%20d%15d%13d%18d%11d\n",
            threads[i].p_id, threads[i].arr_time, threads[i].burst_time,
            current_time, threads[i].turn_around_time, threads[i].waiting_time);
    }

    // Calculate and display the average waiting time and average turn-around time
    double avg_waiting_time = 0;
    double avg_turn_around_time = 0;
    for (int i = 0; i < num_threads; i++) {
        avg_waiting_time += threads[i].waiting_time;
        avg_turn_around_time += threads[i].turn_around_time;
    }

    avg_waiting_time /= num_threads;
    avg_turn_around_time /= num_threads;

    printf("\nAverage Waiting Time: %.2lf\n", avg_waiting_time);
    printf("Average Turn-Around Time: %.2lf\n", avg_turn_around_time);

    return 0;
}
