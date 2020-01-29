#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <math.h>
#include <errno.h>
#include "eratosthenes.h"

int main(int argc, char *argv[]) {
    // Turning off sigpipe
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    // check input = positive int    
    if ((argc != 2)) {
        fprintf(stderr, "Usage:\n\tpfact n\n");
        exit(1);
    }
    char *junk = NULL;   
    if (strtol(argv[1] , NULL, 10) > INT_MAX || strtol(argv[1], NULL, 10) <= 0){
        fprintf(stderr, "Usage:\n\tpfact n\n");
        exit(1);
    }
   
    int n = strtol(argv[1], &junk, 10);
    if (n < 0 || *junk != '\0') {
       fprintf(stderr, "Usage:\n\tpfact n\n");
       exit(1);
    }
    int fd[2];

    // create a pipe
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(2);
    }

    int ret = fork();
    int pid = 0;
    if (ret < 0) {
        perror("fork");
        exit(2);
    }else if (ret > 0){ //parent
        // parent wont be reading from the pipe
        if (close(fd[0]) == -1){
            perror("close");
            exit(2);
        }
        // write input list 2 ... n to pipe
        for (int i=2 ; i <= n; i++) {
            if (write(fd[1], &i, sizeof(int)) < 0){
                if (errno == EPIPE){
                    break;
                }
                else {
                    perror("write");
                    exit(2);
                }
            }
        }
        // close write after using it
        if (close(fd[1]) == -1){
            perror("close");
            exit(2);
        }
        int status;
        if (wait(&status) != -1)  {
            if (WIFEXITED(status)) {
                if (WEXITSTATUS(status) >= 0) {
                    printf("Number of filters = %d\n", WEXITSTATUS(status));
                }
                else {
                    fprintf(stderr, "[%d] Child exited abnormally\n", getpid());
                    exit(2);
                }
            } else {
                fprintf(stderr, "[%d] Child exited abnormally\n", getpid());
                exit(2);
            }
         }
    }else {// child
        //child wont be writing to original pipe
        if (close(fd[1]) == -1){
            perror("close");
            exit(-1);
        }
        int m = 2;
        int *fds = malloc(sizeof(int) * 2);
        int input = fd[0];
        int fact1 = 0;
        int fact2 = 0;
        int num_filters = 0;
        int x = 0;
        while(1) {
            if (m >= sqrt(n)) {
                if (n == m) {
                    printf("%d is prime\n", n);
                }
                else if (m * m == n) {
                    fact1 = m;
                    fact2 = m;
                    printf("%d %d %d\n", n, m, m);
                }
                // one factor between 0 ... sqrtn
                else if (fact1 != 0){
                    fact2 = n/fact1;
                    printf("%d %d %d\n", n, fact1, fact2);
                }
                // no factors between 0 ... sqrtn
                else if (fact1 == 0 && fact2 == 0) {
                    printf("%d is prime\n", n);
                }
                // two distinct factors
                else if (fact1 != 0 && n % m == 0) {
                    fact2 = m;
                    printf("%d %d %d\n", n, fact1, fact2);
                }
                if (close(fds[0]) == -1){
                    perror("close");
                    exit(-1);
                }
                exit(num_filters);
            }
            // check next filter
            if ((n/m) * m == n) {
                if (((n/m) % m) == 0) {
                    printf("%d is not the product of two primes\n", n);
                    exit(num_filters);
                }
            }
            if (n % m == 0) {
                if (fact1 == 0){
                    fact1 = m;
                }
                else {
                    printf("%d is not the product of two primes\n", n);
                    exit(num_filters);
                }
            }
            num_filters = num_filters + 1;
            pid = make_stage(m, input, &fds);
            if (pid < 0) {
                exit(-1);
            }else if (pid > 0) {//child
                break;
            }else {//grandchild
                num_filters = 0;
                // next m
                if ((x = read(fds[0], &m, sizeof(int))) <= 0){
                    if (x == 0) {
                       return 0;
                    }
                    else {
                        perror("read");
                        exit(-1);
                    }
                }
            }
            input = fds[0];
        }
        if (pid > 0) {
            // wait for grand children to finish
            int status;
            if (wait(&status) != -1) {
                if (WIFEXITED(status)) {
                    if (WEXITSTATUS(status) >= 0) {
                        exit(num_filters + WEXITSTATUS(status));
                    }
                    else {
                        fprintf(stderr, "ERROR: grand child process had error");
                        exit(-1);
                    }
                }else {
                    printf("[%d] Grand Child exited abnormally\n", getpid());
                    exit(-1);
                }
            }else {
                perror("wait");
                exit(-1);
            }
        }
    }
    return 0;     
}
