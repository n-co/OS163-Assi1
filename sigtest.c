#include "types.h"
#include "stat.h"
#include "user.h"

int
sig_handler_1(int signum){
  printf(1, "this is signal handler #1. signum = %d\n", signum);
  return 1;
}

int
sig_handler_2(int signum){
  printf(1, "this is signal handler #2. signum = %d\n", signum);
  return 2;
}

int
sig_handler_3(int signum){
  printf(1, "this is signal handler #3. signum = %d\n", signum);
  return 3;
}

int
main(void)
{
  printf(1, "signal test - start\n");
  int parentpid = getpid();
  int i, pid;
  for(i=0; i<1; i++){
    pid = fork();
    if(pid == 0){
      //signal(1,(sighandler_t)(&sig_handler_1));
              printf(1, "#####################################\n");
      while(1){
      sigsend(parentpid,2);    

      }
    }
    else{
              printf(1, "*************************************\n");

      while(1){
        sigsend(pid,1);
      }
      wait(0);
    }
  }  
  exit(0);
}
