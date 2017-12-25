#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include "stats.h"
#define SHM_NAME "thirunavukka_nagamani"
// ADD NECESSARY HEADERS

// Mutex variables
pthread_mutex_t* mutex;
 stats_t *temp;
int maxClients=0;
void exit_handler(int sig) {
	// ADD

	// critical section begins
	pthread_mutex_lock(mutex);

	// Client leaving; needs to reset its segment   
	temp->is_valid_segment=0;
	pthread_mutex_unlock(mutex);
	// critical section ends

	exit(0);
}

int main(int argc, char *argv[]) {
	// ADD    

	int i; 
	stats_t *p;
	struct sigaction act;
	act.sa_handler=exit_handler;
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGTERM, &act, NULL);
	pid_t pid;
	time_t current_time,timer=0,ntimer=0;
	char* c_time_string;
	struct timeval end_t, start_t;
	gettimeofday(&start_t,NULL);
	int fd_shm = shm_open(SHM_NAME, O_RDWR, 0660);
	if(fd_shm==-1)
	{
		exit(1);
	}

	int PAGE_SIZE=getpagesize();
	maxClients=(PAGE_SIZE/64)-1;
	void *shm_ptr= mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
	mutex=(pthread_mutex_t *)shm_ptr;

	// critical section begins
	temp=(stats_t *)shm_ptr;
	for(i=1;i<=maxClients;i++)
	{
		temp++;
		if(temp->is_valid_segment==0)
		{
			pthread_mutex_lock(mutex);
		        temp->is_valid_segment=1;
			pthread_mutex_unlock(mutex);
			break;
		}
	}
	if(i>63)
	{
		exit(0);
	}

	// critical section ends
	pid = getpid();
	temp->pid = pid;
	current_time = time(NULL);
	c_time_string = ctime(&current_time);
	strncpy(temp->birth, c_time_string,24);
        strncpy(temp->clientString, argv[1],9);
	while (1) {
		gettimeofday(&end_t,NULL);
		timer = end_t.tv_sec - start_t.tv_sec;
		ntimer = (end_t.tv_usec - start_t.tv_usec);
		temp->elapsed_sec=(int)timer;
		temp->elapsed_msec=(double)ntimer/1000;
		sleep(1);
                p= (stats_t *) shm_ptr;
                p++;
		printf("Active clients :");
                for (i = 1; i <= maxClients; i++) {
                        if (p->is_valid_segment==1) {
                                printf(" %d",p->pid);
                        }
                        p++;
                }
		printf("\n");
	}
	return 0;
}

