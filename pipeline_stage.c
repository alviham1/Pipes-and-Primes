#include <stdlib.h>
#include <stdio.h>
#include "eratosthenes.h"
#include <errno.h>

// Create a child process, then filter the incoming
//values and send them to that child process
//it's the read end from a previously created pipe
pid_t make_stage(int m, int read_fd, int **fds){
    //TASK(a) create pipe between current stage and next stage
    if (pipe(*fds) == -1) {
        perror("pipe");
        exit(-1);
    }
    //TASK(b): fork to set up new stage or print final result
    int r = fork();
    if (r > 0) { // parent
       // Parent won't be reading from pipe
       if ((close((*fds)[0])) == -1) {
           perror("close");
           exit(-1);
       }
       //TASK(c): parent filters incoming values and writes to child
       if (filter(m, read_fd, (*fds)[1]) == 1) {
           exit(-1);
       }
       //close read_fd after use
       if ((close(read_fd)) == -1) {
           perror("close");
           exit(-1);
       }
       // close write fd after using it
       if ((close((*fds)[1])) == -1) {
           perror("close");
           exit(-1);
       }
       return r;
    }else if (r == 0) {// child
       // Child wont be writing to parents pipe
       if ((close((*fds)[1])) == -1) {
           perror("close");
           exit(-1);
       }
       if ((close(read_fd)) == -1) {
           perror("close");
           exit(-1);
       }
       return 0;

    }else {
         perror("fork");
         exit(-1);
    }

}
 
