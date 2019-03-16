#include <stdlib.h>
#include <stdio.h>
/*
By Nick Di Loreto
for use for csc 209 a3 2018 @ utm
this is used to generate from 2..n; n being argv1
and argv2 beaing teh file to wtire to
*/
int main(int argc, char const *argv[]) {

  if (argc!=3){
    fprintf(stderr, "Usage:\n\tgenerate n , fileName.dat\n");
    exit(1);
  }
  int n=strtol(argv[1],NULL,10);

  char cmd[50];
  for(int i=2; i<=n;i++){
    sprintf(cmd,"./pfact %d >> %s",i,argv[2]);
    system(cmd);
  }

  return 0;
}

