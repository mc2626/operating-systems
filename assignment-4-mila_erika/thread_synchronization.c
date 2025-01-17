/*
 -------------------------------------
 File:    thread_synchronization.c
 Project: a04
 Operating Systems: Q2
 -------------------------------------
 Author:  Mila Cvetanovska, Erika Capper
 ID:      210311400, 210349750
 Email:   cvet1400@mylaurier.ca, capp9750@mylaurier.ca
 Version  2023-11-07
 -------------------------------------
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <time.h>
#include <semaphore.h>

sem_t running;
sem_t even;
sem_t odd;
sem_t dummy;

void logStart(char *tID); //function to log that a new thread is started
void logFinish(char *tID); //function to log that a thread has finished its time

void startClock(); //function to start program clock
long getCurrentTime(); //function to check current time since clock was started
time_t programClock; //the global timer/clock for the program

typedef struct thread //represents a single thread, you can add more members if required
{
	char tid[4]; //id of the thread as read from file
	unsigned int startTime;
	int state;
	pthread_t handle;
	int retVal;
} Thread;


// you can add more functions here if required
// I added global boolean variables 
bool first_thread = true;
bool thread_started = false;


int threadsLeft(Thread *threads, int threadCount);
int threadToStart(Thread *threads, int threadCount);
void* threadRun(void *t); //the thread function, the code executed by each thread
int readFile(char *fileName, Thread **threads); //function to read the file content and build array of threads

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Input file name missing...exiting with error code -1\n");
		return -1;
	}

	//you can add some suitable code anywhere in main() if required
	// initilize the threads
	sem_init(&running, 0, 1);
    sem_init(&even, 0, 0);
    sem_init(&odd, 0, 0);
	sem_init(&dummy, 0, 0);

	Thread *threads = NULL;
	int threadCount = readFile(argv[1], &threads);

	startClock();
	
	while (threadsLeft(threads, threadCount) > 0) //put a suitable condition here to run your program
	{
		//you can add some suitable code anywhere in this loop if required

		int i = 0;
		while ((i = threadToStart(threads, threadCount)) > -1) {
			//you can add some suitable code anywhere in this loop if required
			if (first_thread) {
                first_thread = false; //no longer the first thread
                if (threads[i].tid[2] % 2 == 0)
                    sem_post(&even);
                else
                    sem_post(&odd);
            }

			//set the thread states
			threads[i].state = 1;
			threads[i].retVal = pthread_create(&(threads[i].handle), NULL,
					threadRun, &threads[i]);
		}
	}

	// Wait for all threads to finish
	if (!thread_started) {
		int ready = true;
		for (int i = 0; i < threadCount; i++)
			if (threads[i].state == 0)
				ready = false;

		if (ready)
			thread_started = true;
	}

	// Cleanup
    sem_destroy(&running);
    sem_destroy(&even);
	sem_destroy(&dummy);
    sem_destroy(&odd);
	free(threads);

	return 0;
}


int readFile(char *fileName, Thread **threads) //do not modify this method
{
	FILE *in = fopen(fileName, "r");
	if (!in) {
		printf(
				"Child A: Error in opening input file...exiting with error code -1\n");
		return -1;
	}

	struct stat st;
	fstat(fileno(in), &st);
	char *fileContent = (char*) malloc(((int) st.st_size + 1) * sizeof(char));
	fileContent[0] = '\0';
	while (!feof(in)) {
		char line[100];
		if (fgets(line, 100, in) != NULL) {
			strncat(fileContent, line, strlen(line));
		}
	}
	fclose(in);

	char *command = NULL;
	int threadCount = 0;
	char *fileCopy = (char*) malloc((strlen(fileContent) + 1) * sizeof(char));
	strcpy(fileCopy, fileContent);
	command = strtok(fileCopy, "\r\n");
	while (command != NULL) {
		threadCount++;
		command = strtok(NULL, "\r\n");
	}
	*threads = (Thread*) malloc(sizeof(Thread) * threadCount);

	char *lines[threadCount];
	command = NULL;
	int i = 0;
	command = strtok(fileContent, "\r\n");
	while (command != NULL) {
		lines[i] = malloc(sizeof(command) * sizeof(char));
		strcpy(lines[i], command);
		i++;
		command = strtok(NULL, "\r\n");
	}

	for (int k = 0; k < threadCount; k++) {
		char *token = NULL;
		int j = 0;
		token = strtok(lines[k], ";");
		while (token != NULL) {
//if you have extended the Thread struct then here
//you can do initialization of those additional members
//or any other action on the Thread members
			(*threads)[k].state = 0;
			if (j == 0)
				strcpy((*threads)[k].tid, token);
			if (j == 1)
				(*threads)[k].startTime = atoi(token);
			j++;
			token = strtok(NULL, ";");
		}
	}
	return threadCount;
}

void logStart(char *tID) {
	printf("[%ld] New Thread with ID %s is started.\n", getCurrentTime(), tID);
}

void logFinish(char *tID) {
	printf("[%ld] Thread with ID %s is finished.\n", getCurrentTime(), tID);
}

int threadsLeft(Thread *threads, int threadCount) {
	int remainingThreads = 0;
	for (int k = 0; k < threadCount; k++) {
		if (threads[k].state > -1)
			remainingThreads++;
	}
	return remainingThreads;
}

int threadToStart(Thread *threads, int threadCount) {
	for (int k = 0; k < threadCount; k++) {
		if (threads[k].state == 0 && threads[k].startTime == getCurrentTime())
			return k;
	}
	return -1;
}

void* threadRun(void *t)
{ //implement this function in a suitable way
	logStart(((Thread*) t)->tid);

	//your synchronization logic will appear here
	if (!thread_started) {
		if(((Thread *)t)->tid[2] == 49) //thread = t01
		{
			sem_wait(&dummy);
		}
        else if (((Thread *)t)->tid[2] % 2){
			sem_wait(&odd);
		}
        else{
			sem_wait(&even);
		}
        sem_wait(&running);
    }

	//critical section starts here
	printf("[%ld] Thread %s is in its critical section\n", getCurrentTime(),
			((Thread*) t)->tid);
	//critical section ends here

	// your synchronization logic will appear here
	if (thread_started) {
		int value;

        sem_getvalue(&even, &value);
        if (value == 0){
			sem_post(&even);
		}

        sem_getvalue(&odd, &value);
        if (value == 0){
			sem_post(&odd);
		}
        sem_post(&running);
	} 
	else{
        if(((Thread *)t)->tid[2] == 55){ //thread = t07
			sem_post(&dummy);
		}
		if(((Thread *)t)->tid[2] == 49){ //thread = t01
			sem_post(&odd);
		}
        else if(((Thread *)t)->tid[2] % 2){
			sem_post(&even);
		}
        else{
			sem_post(&odd);
		}
        sem_post(&running);
	}

	logFinish(((Thread*) t)->tid);
	((Thread*) t)->state = -1;
	pthread_exit(0);
    
} 

void startClock() {
	programClock = time(NULL);
}

long getCurrentTime() //invoke this method whenever you want check how much time units passed
//since you invoked startClock()
{
	time_t now;
	now = time(NULL);
	return now - programClock;
}

