#include <semaphore.h>
#include <sys/types.h>
#include <fcntl.h>
#ifndef SRC_STATS_H_
#define SRC_STATS_H_
#endif /* SRC_STATS_H_ */

typedef struct {
    int pid;
    char birth[25];
    char clientString[10];
    int elapsed_sec;
    double elapsed_msec;
    int is_valid_segment;
} stats_t;
