#include <stdlib.h>
#include <stdio.h>

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

