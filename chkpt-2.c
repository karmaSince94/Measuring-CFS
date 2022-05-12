#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <math.h>
#include <stdint.h>	

#define BILLION 1000000000L

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct data_t {
    int id;
    int sleep;
};

void* thread(void *arg)
{
    struct data_t *data = arg;
    long double sum = 0.0;
    long double term, pi;

    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);

    struct timeval tv;
    struct timespec start, end; 
    clock_gettime(CLOCK_REALTIME, &start);
    uint64_t diff;
    FILE *fpt;
    for(long int i=0;i< 1000000000;i++) {
        term = pow(-1, i) / (2*i+1);
        sum += term;
    }
    pi = 4 * sum;
    clock_gettime(CLOCK_REALTIME, &end);
    diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    fpt = fopen("Result1.csv", "a");
    fprintf(fpt,"%llu, %llu, %llu, %d, %d\n",(long long unsigned int) start.tv_nsec, (long long unsigned int) end.tv_nsec, (long long unsigned int) diff, data->id, getpid());
}

int main(void) {

    struct data_t data[9];
    pthread_t ths[9];

    srand(time(NULL));

    for(int i = 0; i < 9; ++i)
    {
        data[i].id = i + 1;
        data[i].sleep = 1 + rand() % 6;
        pthread_create(ths + i, NULL, thread, data + i);
    }
    // give time for all threads to lock
    sleep(1);
    pthread_cond_broadcast(&cond);
    for(int i = 0; i < 9; ++i)
        pthread_join(*(ths + i), NULL);
    return 0;
}
