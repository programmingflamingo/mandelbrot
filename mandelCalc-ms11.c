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

  signal( SIGUSR1, usr1_handler );



  struct msgbuf sbuf1;
  size_t len;

  while(1)
  {
    //a
    fscanf(stdin, "%lf %lf %lf %lf %d %d %d", &xMin, &xMax, &yMin, &yMax, &row, &col, &numIter);

    //b
    char *data = shmat(sid, 0, 0);

    double Cy, Cx, Zx, Zy;
    double deltaX, deltaY;
    double Zx_next, Zy_next;

    deltaX = (xMax-xMin)/(col-1);
    deltaY = (yMax-yMin)/(row-1);

    int n, r, c;
    for(r = 0; r < row; r++) {
      Cy = yMin + r * deltaY;
      for(c = 0; c < col ; c++) {

        Cx = xMin + c * deltaX;
        Zx = 0.0;
        Zy = 0.0;

        for( n = 0; n < numIter; n++ ) {
          if( (Zx * Zx) + (Zy * Zy) >= 4.0)
            break;

           Zx_next = (Zx * Zx) - (Zy * Zy) + Cx;
           Zy_next = (2.0 * Zx * Zy) + Cy;
           Zx = Zx_next; 
           Zy = Zy_next;
        }
        if( n >= numIter) 
          *( data + r * col + c ) = -1;
        else
          *( data + r * col + c ) = n;
      }
    }

    //c
    fprintf(stdout, "%lf %lf %lf %lf %d %d %d\n", xMin, xMax, yMin, yMax, row, col, numIter);
    fflush(stdout);

    //d
    sbuf1.mtype=1;
    strcpy(sbuf1.mtext, "1\n"); 

    len = strlen(sbuf1.mtext)+1;

    if(msgsnd(mid1, &sbuf1, len, 0)==-1)
      PrintE("msgsnd");
   
    numimages++;
  }
  return 1;
}
