#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>
#include <stdint.h>	
#define BILLION 1000000000L
static volatile sig_atomic_t got_signal;

void printPi(int sig) {
    got_signal = 1;
    uint64_t diff;
    struct timespec start, end;
    double elapsed;
    clock_gettime(CLOCK_MONOTONIC, &start);
    clock_t sstart, send;
    sstart = clock();
    printf("Hello World! It's me, child  #%d! that received %d\n", getpid(), sig);
    long int i, n;
    long double sum = 0.0;
    long double term, pi;
    int pid = getpid();

    for(i=0;i< 1000000000;i++) {
        term = pow(-1, i) / (2*i+1);
        sum += term;
    }
    pi = 4 * sum;

    printf("\nPI = %.9LF\n", pi);
    clock_gettime(CLOCK_MONOTONIC, &end);
    send = clock();
    double time_taken = ((double) (send - sstart))/(double)CLOCKS_PER_SEC;
    diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    FILE *fpt;
    fpt = fopen("MyFile.csv", "a");
    fprintf(fpt,"%llu, %llu, %llu, %d\n",(long long unsigned int) start.tv_nsec, (long long unsigned int) end.tv_nsec, (long long unsigned int) diff, pid);
}

int main(int argc, char *argv[]) {
    int n_task = 4;
    int status;
    pid_t pid;
    pid_t pid_array[n_task];
    
    signal(SIGUSR1, printPi);

    for (int i = 0; i < n_task; i++)
    {
        pid = fork();

        if (pid < 0)
        {
            perror("fork");
            exit(1);
        }

        if (pid == 0) //child
        {
            printf("printed from child [%d]\n", getpid());
            while (!got_signal);
            exit(0);
        }
        
         pid_array[i] = pid;
    } 
        sleep(1);
        
    for (int i =0; i < n_task; i++) {
        kill(pid_array[i], SIGUSR1);
    }

    for (int i = 0; i < n_task; i++)
        waitpid(pid_array[i], &status, 0);

    return (0);
}