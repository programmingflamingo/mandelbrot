#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define INr  0
#define OUTw 1

int numimages             = 0;
int signalCaughtusr1      = 0;

  int row, col, numIter;
  double xMin, xMax, yMin, yMax; 

struct msgbuf {
    long mtype;
    char mtext[256]; 
};

void usr1_handler( int sig ) {
  signalCaughtusr1 = sig;
  exit(numimages);
}

void PrintE(char *str) {
  perror (str);
  exit (EXIT_FAILURE);
}

int main( int argc, char ** argv )
{
  int sid = atoi(argv[1]);
  int mid1 = atoi(argv[2]);
  int mid2 = atoi(argv[3]);

  signal( SIGUSR1, usr1_handler );

  struct msgbuf sbuf1;
  struct msgbuf sbuf2;
  size_t len;

  FILE* fp;

  while(1)
  {
    //a 
    fscanf(stdin, "%lf %lf %lf %lf %d %d %d", &xMin, &xMax, &yMin, &yMax, &row, &col, &numIter);

    //b
    if(msgrcv(mid2, &sbuf2, 256, 0, 0)==-1)
      PrintE("C2:msgsnd");


    if((fp = fopen(sbuf2.mtext, "w"))==NULL)
      PrintE("fopen");

    //c-d
    char *data = shmat(sid, 0, 0);
    char colors[ 15 ] = ".-~:+*%O8&?$@#X";


    for(int r = row - 1; r >= 0; r--) {
      for(int c = 0; c < col ; c++) {
        int n = *( data + r * col + c );
        if( n < 0 ) {
          fprintf(stderr," ");
          fprintf(fp,"%d ", -1);
        }
        else {
          fprintf(stderr,"%c", colors[n%15]);
          fprintf(fp,"%d ", n);
        }
      }
      fprintf(stderr,"\n");
      fprintf(fp, "\n");
    }

    //e
    fclose(fp);

    //f
    sbuf1.mtype=2;
    strcpy(sbuf1.mtext, "2\n"); 

    len = strlen(sbuf1.mtext)+1;

    if(msgsnd(mid1, &sbuf1, len, 0)==-1)
      PrintE("msgsnd");

    numimages++;
  }
  return 2;
}
