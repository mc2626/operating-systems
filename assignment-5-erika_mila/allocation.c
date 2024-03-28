/*
 -------------------------------------
 File:    allocation.c
 Project: a05
 Operating Systems: Q2
 -------------------------------------
 Author:  Mila Cvetanovska, Erika Capper
 ID:      210311400, 210349750
 Email:   cvet1400@mylaurier.ca, capp9750@mylaurier.ca
 Version  2023-11-21
 -------------------------------------
 */

// PROBLEMS -->
// 
// - compaction() function ('C' command) isn't fully correct
// - Debugging lines aren't fully correct:
//
//      index = 0 delta = 8000 best delta = 1048577
//      index = 1 delta = 78576 best delta = 80000
//
//      index = 0 delta = 5000 best delta = 1048577

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#define MAX 10

char cmd[3];
char pid[3];
char sort_type[2];
size_t space;
size_t total_space = 0;
int statusFlag = 0;

// Create a struct similar to linked list to store memory blocks.
// Contains next, previous pointers, starting and ending memory addresses and the pid 
typedef struct BlockMem
{
    struct BlockMem *prev,*next;
    size_t start_mem, end_mem;
    char *processid;
} BlockMem;

//init memory block that will be used in main
BlockMem *memory;
// init memory size to zero
size_t starting_mem = 0;

//Function to store user input into a memory block struct
BlockMem *allocate(char *pid, size_t start_mem, size_t end_mem, BlockMem *prev, BlockMem *next){
   
    // allocate memory, and store all data that we recieved from the user into the 
    // structure. Set up the linked list and insert accordingly.
    BlockMem *m = malloc(sizeof(BlockMem));
    m->start_mem = start_mem;
    m->end_mem = end_mem;

    if (strlen(pid) != 0){
        m->processid = malloc(sizeof(char)*(strlen(pid)+1));
        strcpy(m->processid,pid);
    } else {
        m->processid = NULL;
    }
    m->next = next;
    m->prev = prev;
    if (prev)
        prev->next = m;
    if (next)
        next->prev = m;
    return m;
}

void get_request(char *pid, char *sort_type, size_t mem_size){
    // Search for smallest hole in memory by traversing
    // the linked list
    BlockMem *h = NULL;
    // size_t best_delta = starting_mem;  // Initialize with the maximum possible value

    if (strcmp(sort_type,"B") == 0){
        BlockMem *iter = memory;
        size_t min = -1;
        // size_t index = 0;

        while(iter){
            size_t hsize = (iter->end_mem - iter->start_mem + 1);
            if (iter->processid == NULL && mem_size<=hsize && hsize < min ){
                min = hsize;
                h = iter;
                // size_t delta = hsize - mem_size;
                // printf("index = %zu delta = %zu best delta = %zu\n", index, delta, best_delta);
                // if (delta < best_delta) {
                //     best_delta = delta;
                //     h = iter;
                //     printf("index = %zu delta = %zu best delta = %zu\n", index, delta, best_delta);
                // }
            }
            iter = iter->next;
            // index++;
        }
    }else{
        printf("Invalid Input\n");
        return;
    }
    if(!h || h->processid != NULL){
        printf("No hole of sufficient size\n");
        return;
    } else {
        total_space += mem_size;
    }
    h->processid = malloc(sizeof(char)*strlen(pid)+1);
    strcpy(h->processid,pid);
    if(h->end_mem - h->start_mem + 1 == mem_size){
        return; 
    }
    h->next = allocate("",h->start_mem + mem_size,h->end_mem,h,h->next);
    h->end_mem = h->start_mem + mem_size - 1;
    printf("Successfully allocated %ld to process %s\n",mem_size,pid);
    return;
}

int release(char *pid){
    printf("Releasing memory for process %s\n",pid);
    int flag = 1;
    BlockMem *iter = memory;
    while(iter){
        if(iter->processid && strcmp(iter->processid,pid) == 0){
            total_space -= (iter->end_mem - iter->start_mem); 
            free(iter->processid);
            iter->processid = NULL;
            flag = 0;
        }
        if(iter->processid == NULL && iter->prev && iter->prev->processid == NULL){
            BlockMem *t = iter->prev;
            iter->prev = t->prev;
            if(t->prev){
                t->prev->next = iter;
            }
            iter->start_mem = t->start_mem;
            free(t);
        }
        if(iter->prev == NULL){
            memory = iter;
        }
        iter = iter->next;
    }
    if(flag){
        printf("Nothing released\n");
    }
    printf("Successfully released memory for process %s\n",pid);
    return flag;
}

void get_status(){
    BlockMem *iter = memory;
    if (statusFlag == 0){
        printf("Partitions [Allocated Memory = %06zu]\n",total_space-1);
        while(iter) {
            if(iter->processid) {
                printf("Address [%06zu : %06zu] ", iter->start_mem, iter->end_mem);
                printf("Process %s\n", iter->processid);
            }
            iter=iter->next;
        }
    }
    if (statusFlag == 1){
        printf("Holes [Free Memory = %06zu]\n",(starting_mem - total_space + 1));
        while(iter) {
            if(!iter->processid) {
                printf("Address[%06zu : %06zu] len = %zu\n", iter->start_mem, iter->end_mem, (iter->end_mem - iter->start_mem + 1));
            }
            iter=iter->next;
        }
    }
}

void compaction()
{
   
    BlockMem *iter = memory;
    size_t new_start = 0;

    // Update starting and ending addresses of allocated memory blocks
    while (iter)
    {
        if (iter->processid) // Skip allocated memory blocks
        {
            iter->start_mem = new_start;
            iter->end_mem = new_start + (iter->end_mem - iter->start_mem);
            new_start = iter->end_mem + 1;
        }
        iter = iter->next;
    }

    // Remove all holes except the last one
    iter = memory;
    while (iter->next)
    {
        if (!iter->processid)
        {
            BlockMem *temp = iter;
            if (iter->prev)
            {
                iter->prev->next = iter->next;
            }
            iter->next->prev = iter->prev;
            iter = iter->next;
            free(temp);
        }
        else
        {
            iter = iter->next;
        }
    }

    // Update the last hole
    if (iter->processid == NULL)
    {
        iter->start_mem = new_start;
        iter->end_mem = starting_mem - 1;
    }
    else
    {
        // Add a new hole at the end
        iter->next = allocate("", new_start, starting_mem - 1, iter, NULL);
    }
}

int main(int argc, char *argv[])
{
    sscanf(argv[1], "%zu", &starting_mem);
    memory = allocate("",0,starting_mem-1,NULL,NULL);
    printf("Here, the Best Fit Appraoch has been implemented and the allocated %ld bytes of memory.\n", starting_mem);
    printf("allocator>");
    
    char command[128];
    fflush(stdout);
    read(0,command,128);

    while (strcmp(command,"Exit")!= 0)
    {
        sscanf(command, "%s %s %ld %s", cmd, pid, &space, sort_type);

        // Request
        if (strcmp("RQ", cmd) == 0){ 
            get_request(pid,sort_type,space);
        
        }
        // Release memory
        else if (strcmp("RL", cmd) == 0){ 
            release(pid);
        }
        // Status report
        else if (strcmp("Status", cmd) == 0){ 
            get_status();
            statusFlag = 1;
            get_status();
            statusFlag = 0;
        }
        // Compaction
        else if (strcmp("C", cmd) == 0){ 
            compaction();
            printf("Compaction process is successful\n");
        }
        // Exit
        else if (strcmp("Exit", cmd) == 0){ 
            break;
        }
        //Invalid input
        else{
            printf("This command is invalid. Please enter RQ, RL, C, Status or Exit\n");
        }

        printf("allocator>");
        fflush(stdout);
        read(0, command, 128);

        // Remove trailing newline character
        command[strcspn(command, "\n")] = '\0';
            
    }
}