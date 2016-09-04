#include "types.h"
#include "stat.h"
#include "perf.h"
#include "user.h"
#include "syscall.h"

void cpuonly(void){
  int tick = uptime();
  int prev = tick;
  int counter = 0;
  while(counter<30){
    tick = uptime();
    if(tick != prev){
      counter++;
      prev = tick;
    }
  }
  exit(0);
}

void blockonly(void){
  int tick = uptime();
  int prev = tick;
  int counter = 0;
  while(counter<30){
    tick = uptime();
    if(tick != prev){
      counter++;
      prev = tick;
      sleep(1);
    }
  }
  exit(0);
}

void mixed(void){
  int i;
  for(i=0; i<5; i++){
    int tick = uptime();
    int prev = tick;
    int counter = 0;
    while(counter<5){
      tick = uptime();
      if(tick != prev){
        counter++;
        prev = tick;
        sleep(1);
      }
    }
  }
  exit(0);
}

int
main(void)
{
  printf(1, "sanity test - start\n");
  int i, pid;
  struct perf average;
  int avg_turnaround = 0;
  struct perf curr;

  average.stime  = 0;
  average.retime = 0;
  average.rutime = 0;


  for(i=0; i<10; i++){
    pid = fork();
    if(pid==0)
      cpuonly();
  }
  for(i=0; i<10; i++){
    pid = fork();
    if(pid==0)
      blockonly();
  }
  for(i=0; i<10; i++){
    pid = fork();
    if(pid==0)
      mixed();
  }

  int currpid;
  for(i=0; i<30; i++){
    currpid = wait_stat(0,&curr);
    printf(1, "child process id: %d is finished\n",currpid);
    printf(1, "waiting time:     %d \n",curr.retime);  // maybe changed to retime+stime
    printf(1, "sleeping time:    %d \n",curr.stime);
    printf(1, "running time:     %d \n",curr.rutime);
    printf(1, "turnaround time:  %d \n\n",(curr.ttime-curr.ctime));
    avg_turnaround += (curr.ttime - curr.ctime);
    average.stime  += curr.stime; 
    average.retime += curr.retime;
    average.rutime += curr.rutime;
  }

    printf(1, "sanity test - end\n");
    printf(1, "average:\n");
    printf(1, "waiting time:     %d \n",average.retime/30);  // maybe changed to retime+stime
    printf(1, "sleeping time:    %d \n",average.stime/30);
    printf(1, "running time:     %d \n",average.rutime/30);
    printf(1, "turnaround time:  %d \n\n",avg_turnaround/30);

  exit(0);
}