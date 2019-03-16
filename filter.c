#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "eratosthenes.h"

int filter(int m, int freadfd, int writefd) {
    int buf, n;
    // read an integer from freadfd
    while ((n = read(freadfd, &buf, sizeof(int))) > 0) {
       // if integer is not a multiple of m
       if (buf % m != 0) {
          // write to writefd
          if (write(writefd, &buf, sizeof(int)) == -1){
             if (errno == EPIPE) {
                 break;
             }else {
                 perror("write");
                 return 1;
             }
          }
       }
    }
    if (n < 0) {
       perror("read");
       return 1;
    }
    return 0;
}
 
