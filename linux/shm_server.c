#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include "stats.h"
#define SHM_NAME "thirunavukka_nagamani"
// ADD NECESSARY HEADERS

// Mutex variables

pthread_mutex_t mutex;
pthread_mutexattr_t mutexAttribute;
int PAGE_SIZE;
void *shm_ptr;
int maxClients=0;
void exit_handler(int sig) 
{
	// ADD
	munmap(shm_ptr,PAGE_SIZE);
	shm_unlink(SHM_NAME);

	exit(0);
}

int main(int argc, char *argv[]) 
{
	// ADD
	int counter=1;
	stats_t *temp;
	int i;
	struct sigaction act;
        act.sa_handler=exit_handler;
        sigaction(SIGINT, &act, NULL);
        sigaction(SIGTERM, &act, NULL);
	// Creating a new shared memory segment
	int fd_shm = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0660);	

	if(fd_shm == -1)
	{
		exit(1);
	}
	PAGE_SIZE=getpagesize();
	int maxClients=(PAGE_SIZE/64)-1;
	ftruncate(fd_shm, PAGE_SIZE);
	shm_ptr= mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
	
	// Initializing mutex
	pthread_mutexattr_init(&mutexAttribute);
	pthread_mutexattr_setpshared(&mutexAttribute, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&mutex, &mutexAttribute);
	memcpy(shm_ptr, &mutex, sizeof(pthread_mutex_t));

	while (1) 
	{
		 temp = (stats_t *) shm_ptr;
		for (i = 1; i <= maxClients; i++) {
			temp++;
			if (temp->is_valid_segment==1) {
				printf("%d, pid : %d, birth : %s, elapsed : %d s %0.4lf ms, %s\n", counter, temp->pid, temp->birth, temp->elapsed_sec, temp->elapsed_msec, temp->clientString);
			}
		}
		counter++;
		sleep(1);
	}
	return 0;
}
