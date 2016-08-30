#include "types.h"
#include "stat.h"
#include "user.h"

void
sig_handler_1(int signum){
  printf(1, "this is signal handler #1. signum = %d, pid = %d\n", signum, getpid());
}

void
sig_handler_2(int signum){
  printf(1, "this is signal handler #2. signum = %d, pid = %d\n", signum, getpid());
}

void
sig_handler_3(int signum){
  printf(1, "this is signal handler #3. signum = %d, pid = %d\n", signum, getpid());
}

int
main(void)
{
  printf(1, "signal test - start\n");
  int parentpid = getpid();
  int pid;
  signal(13,sig_handler_3);
  pid = fork();
  if(pid==0){
  	//child
  	printf(1, "child\n");
  	sigsend(parentpid, 13);
  	printf(1, "child end\n");
  	exit(0);
  }
  else{
  	//parent
  	sigsend(pid, 20);
  	printf(1, "parent\n");
  	wait(0);
  	
  	printf(1, "parent end\n");
  }
  exit(0);
}
