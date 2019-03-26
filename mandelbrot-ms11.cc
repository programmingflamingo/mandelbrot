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

int signalCaughtchld      = 0;

struct msgbuf {
    long mtype;
    char mtext[256]; 
};

void chld_handler( int sig ) {
  signalCaughtchld = sig;
  return;
}

void PrintE(char *str) {
  perror (str);
  exit (EXIT_FAILURE);
}

int main( int argc, char ** argv )
{ 
  fprintf(stderr, "CS 361 HW #4\n");
  fprintf(stderr, "Name  : Michael Slomczynski\n");
  fprintf(stderr, "UIN   : 660304702\n");
  fprintf(stderr, "NetID : ms11\n");

  int row, col, numIter;
  double xMin, xMax, yMin, yMax; 
  char filename[256];

  int pipe_1[2];
  int pipe_2[2]; 

  int mid1;
  int mid2;

  struct msgbuf sbuf1;
  struct msgbuf sbuf2;
  size_t len;

  int sid;

  //1-4//
  /**************************************************/
  //1
  if (pipe(pipe_1)==-1)
   PrintE("pipe1");

  if (pipe(pipe_2)==-1)
    PrintE ("pipe2");

  //2
  if ((mid1 = msgget(IPC_PRIVATE, IPC_CREAT | 0666 ))==-1) 
    PrintE("msgget(mid1)");

  if ((mid2 = msgget(IPC_PRIVATE, IPC_CREAT | 0666 ))==-1) 
    PrintE("msgget(mid2)");

  //3
  if ((sid = shmget(IPC_PRIVATE, 20000*sizeof(int), IPC_CREAT | 0666))==-1)
    PrintE("P:shmget");

  //4
  signal( SIGCHLD, chld_handler );
  /**************************************************/

  pid_t pidc1;
  pid_t pidc2;

  pidc1 = fork ();
  
  /**************************************************
  |1: Fork 1 Failed                                 |
  **************************************************/ 
  if (pidc1 < 0)
    PrintE ("C1:fork");

  /**************************************************
  |2: Child Process 1                               |
  **************************************************/  
  else if (pidc1 == 0) {

    //Child2's Pipes//
    /**************************************************/
    if(dup2(pipe_1[INr], STDIN_FILENO)==-1)
      PrintE("C1:dup2(IR)");

    if(dup2(pipe_2[OUTw], STDOUT_FILENO)==-1)
      PrintE("C1:dup2(OW)");

    if(close(pipe_1[INr])==-1)
      PrintE("C1(p1[IR])");

    if(close(pipe_1[OUTw])==-1)
      PrintE("C1(p1[OW])");

    if(close(pipe_2[INr])==-1)
      PrintE("C1(p2[IR])");

    if(close(pipe_2[OUTw])==-1)
      PrintE("C1(p2[OW])");

    //if(close(INr)==-1)
    //  PrintE("C1:close(IR)");

    //if(close(OUTw)==-1)
    //  PrintE("C1:close(OW)");
    /**************************************************/
    char str_mid1[20];
    char str_sid[20];

    sprintf(str_mid1, "%d", mid1);
    sprintf(str_sid, "%d", sid);

    execlp("./mc", "./mc", str_sid, str_mid1, NULL);
    PrintE("Error Exec PID1");
    _exit(-1);
  }

  else  {

    pidc2 = fork ();

    /**************************************************
    |1: Fork 2 Failed                                 |
    **************************************************/ 

    if (pidc2 < 0)
      PrintE ("C2:fork");

    /**************************************************
    |2: Child Process 2                               |
    **************************************************/ 
    else if(pidc2 == 0) {      

      //Child2's Pipes//
      /**************************************************/
      if(dup2(pipe_2[INr], STDIN_FILENO)==-1)
        PrintE("C2:dup2");

      if(close(pipe_1[INr])==-1)
        PrintE("C2(p1[IR])");

      if(close(pipe_1[OUTw])==-1)
        PrintE("C2(p1[OW])");

      if(close(pipe_2[INr])==-1)
        PrintE("C2(p2[IR])");

      if(close(pipe_2[OUTw])==-1)
        PrintE("C2(p2[OW])");

      //if(close(INr)==-1)
        //PrintE("C2:close(IR)");

      //if(close(OUTw)==-1)
      //  PrintE("C2:close(OW)");
      /**************************************************/

      char str_mid1[20];
      char str_mid2[20];
      char str_sid[20];

      sprintf(str_mid1, "%d", mid1);
      sprintf(str_mid2, "%d", mid2);
      sprintf(str_sid, "%d", sid);

      execlp("./md", "./md", str_sid, str_mid1, str_mid2, NULL);
      PrintE("Error Exec PID2");
      _exit(-2);
    }

    /**************************************************
    |3: Parent Process                                |
    **************************************************/ 
    else {

      //Parent's Pipes//
      /**************************************************/
      if(dup2(pipe_1[OUTw],STDOUT_FILENO)==-1)
        PrintE("P:dup2");

      if(close(pipe_1[INr])==-1)
        PrintE("P(p1[IR])");

      if(close(pipe_1[OUTw])==-1)
        PrintE("P(p1[OW])");

      if(close(pipe_2[INr])==-1)
        PrintE("P(p2[IR])");

      if(close(pipe_2[OUTw])==-1)
        PrintE("P(p2[OW])");

      //if(close(INr)==-1)
        //PrintE("P:close(IR)");

      //if(close(OUTw)==-1)
        //PrintE("P:close(OW)");
      /**************************************************/

      while(1) {

        fprintf(stderr, "\n-1 to exit\n");
        
        fprintf(stderr, "row     : ");
        scanf("%d", &row);

        if(row > 0) {
          fprintf(stderr, "col     : ");
          scanf("%d", &col);

          fprintf(stderr, "numIter : ");
          scanf("%d", &numIter);

          fprintf(stderr, "xMin    : ");
          scanf("%lf", &xMin);

          fprintf(stderr, "xMax    : ");
          scanf("%lf", &xMax);

          fprintf(stderr, "yMin    : ");
          scanf("%lf", &yMin);

          fprintf(stderr, "yMax    : ");
          scanf("%lf", &yMax);

          fprintf(stderr, "Filename : ");
          scanf("%255s", filename);

          //if user is not done//
          /**************************************************/
          sbuf2.mtype=1;
          strcpy(sbuf2.mtext, filename); 

          len = strlen(sbuf2.mtext)+1;

          if(msgsnd(mid2, &sbuf2, len, 0)==-1) {
            fprintf(stderr, "%d %ld %s %lu\n", mid2, sbuf2.mtype, sbuf2.mtext, len);
            PrintE("msgsnd");
          }

          fprintf(stdout, "%lf %lf %lf %lf %d %d %d\n", xMin, xMax, yMin, yMax, row, col, numIter);
          fflush(stdout);

          if(msgrcv(mid1, &sbuf1, 256, 0, 0)==-1) {
            PrintE("P:msgrcv1");
          }

          if(msgrcv(mid1, &sbuf1, 256, 0, 0)==-1) {
            PrintE("P:msgrcv2");
          }
          /**************************************************/
        }

        else {
          kill (pidc1, SIGUSR1);
          kill (pidc2, SIGUSR1);

          int status1, status2;
          fprintf(stderr, "\nFinal Results\n");
          (void)waitpid(pidc1, &status1, 0);
  
          if (WIFEXITED( status1 ))
            fprintf(stderr,"  mandelCalc(%d) : Exit Value %d\n", pidc1, WEXITSTATUS(status1));

          (void)waitpid(pidc2, &status2, 0);
  
          if (WIFEXITED( status2 )) 
            fprintf(stderr,"  mandelDisp(%d) : Exit Value %d\n", pidc2, WEXITSTATUS(status2));

          //shmdt(shm);
          shmctl(sid, IPC_RMID, NULL);
          break;
        }
      }
    }
  }

  return 0;
}
