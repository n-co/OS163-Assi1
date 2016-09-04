#include "types.h"
#include "stat.h"
#include "user.h"
#include "perf.h"

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

void
sig_handler_4(int signum){
  printf(1, " TEST 4: testing signal BEFORE change.. \n");
  //printf(1, "before change signal \n");

}

void
sig_handler_5(int signum){
  //printf(1, "after change signal \n");
  printf(1, "********* TEST 4 PASSED - signal changed *********\n");

}



void checkHandler(int signal)
{  
 printf(1,"not default handler on : signal num is: %d \n", signal);
}


void
test_one_fork(){
  int parentpid = getpid();
  int* status=0;
  int pid;
         printf(1, "parent id is: %d\n",parentpid);

  signal(13,sig_handler_3);
  pid = fork();
  if(pid==0){
  	//child
    int childpid = getpid();
  	printf(1, "child %d\n",childpid);
  	sigsend(parentpid, 13);
  	printf(1, "child %d end\n",childpid);
  	exit(0);
  }
  else{
  	//parent
  	//sigsend(pid, 20);
  	printf(1, "parent\n");
  	wait(status);

  	printf(1, "parent end\n");
  	   printf(1, "********* TEST 1 PASSED *********\n");

  }


}

/*void
test_wrong_signal_number(){

  sighandler_t sig_error=signal(300,sig_handler_3);

  if((int)(sig_error) == -1)  
  	printf(1, "********* TEST 2 PASSED *********\n");
  else
  	printf(1, "######### TEST 2 FAILED #########\n");

}*/


/*void
test_wrong_pid(){
//int parentpid=getpid();
  int pid;
  signal(1,sig_handler_4);
  pid = fork();
  if(pid==0){
  	//child
    int sig_status=sigsend(999999, 1);
  	if (sig_status==-1)
  		printf(1, "********* TEST 3 PASSED *********\n");
  	else
  		printf(1, "######### TEST 3 FAILED #########\n");

  	exit(0);
  }
  else{
  	//parent
  	wait(0);
  }
}*/
/*
void
test_change_signal(){
  int parentpid=getpid();
  int pid,pid2;
  signal(5,sig_handler_4);
  pid = fork();
  if(pid==0){
  	//child
  	sleep(10);
    sigsend(parentpid, 5);

  	exit(0);
  }
  else{
  	//parent
  	wait(0);
  	sighandler_t test_change=signal(5,sig_handler_5);
  	if (test_change!=sig_handler_4){
  		printf(1, "######### TEST 4 FAILED #########\n");

  	}
  	pid2=fork();
  	if(pid2==0){
  		sleep(10);
  		sigsend(parentpid, 5);
  		exit(0);
  	}
  	else 
  		wait(0);

  }
  
}


void test_all_signals(){
  printf(1,"start test all signals: \n");
  int parentpid=getpid();
  int i=0;
  for(;i<32;i++)
    signal(i,checkHandler);
  i=0;
  for(;i<32;i++)
    sigsend(parentpid,i);
  printf(1,"ended test all signals!\n");
 
}

void test2(){
  struct perf curr;
  int i,pid,parentpid;
  printf(1,"TEST 2: STARTED\n");
  parentpid=getpid();
  signal(6,checkHandler);
  printf(1, "handler test: %d\n" , checkHandler);
  int id =fork();
  	 if(id==0){
		for (i=0;i<64;i++)
			sigsend(parentpid,4); 
  		exit(0);
  	 }
     else{
        signal(4,checkHandler);
  		pid = wait_stat(0,&curr);
		printf(1, "pid is: %d \n", pid);
  	 }  
  	 printf(1,"TEST 2: ENDED\n");
}*/


int
main(void)
{



  printf(1, "signal test - start\n");

  int i;
  for(i=0; i<100; i++){
  	printf(1,"%d:\n", i);
	  test_one_fork();
	  //test_wrong_signal_number();
	  //test_wrong_pid();

  	//test_change_signal();
  }
  	
  /*int i=0;
  for(;i<3;i++){
	  printf(1,"TEST %d STARTED: \n",i);
	  //test_all_signals();
	  test2();  // ************************* to check *************************
	  printf(1,"TEST %d ENDED\n",i);
  }
  */
    exit(0);
}




