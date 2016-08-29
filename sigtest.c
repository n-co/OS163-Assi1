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
  int pid;
  //signal(13,(sighandler_t)sig_handler_1);
  pid = fork();
  if(pid==0){
  	//child
  	sigsend(parentpid, 13);
  	printf(1, "child end\n");
  	exit(0);
  }
  else{
  	//parent
  	printf(1, "parent\n");
  	printf(1, "wait = %d\n", wait(0));
  	
  	printf(1, "parent end\n");
  }
  exit(0);
}
