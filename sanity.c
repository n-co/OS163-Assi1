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
  int i;
  for(i=0; i<30; i++)
    sleep(1);
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
      }
      else
        printf(1, "NOT CHANGED \n");
    }
    sleep(1);
  }
  exit(0);
}

int
main(void)
{
  printf(1, "sanity test - start\n");
  int i, pid, parentpid = getpid();;
  struct perf average;
  int avg_turnaround=0, g1_turnaround=0, g2_turnaround=0, g3_turnaround=0;
  struct perf curr;
  struct perf g1,g2,g3;

  g1.stime  = 0;
  g1.retime = 0;
  g1.rutime = 0;
  g2.stime  = 0;
  g2.retime = 0;
  g2.rutime = 0;
  g3.stime  = 0;
  g3.retime = 0;
  g3.rutime = 0;
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

    if(parentpid<currpid && currpid<=parentpid+10){
      g1_turnaround += (curr.ttime - curr.ctime);
      g1.stime  += curr.stime; 
      g1.retime += curr.retime;
      g1.rutime += curr.rutime;
    }
    else if(parentpid<currpid+10 && currpid<=parentpid+20){
      g2_turnaround += (curr.ttime - curr.ctime);
      g2.stime  += curr.stime; 
      g2.retime += curr.retime;
      g2.rutime += curr.rutime;
    }
    else if(parentpid<currpid+20 && currpid<=parentpid+30){
      g3_turnaround += (curr.ttime - curr.ctime);
      g3.stime  += curr.stime; 
      g3.retime += curr.retime;
      g3.rutime += curr.rutime;
    } 
  }

    printf(1, "sanity test - end\n");
    
    printf(1, "GROUP 1 - cpu only:\n");
    printf(1, "waiting time:     %d \n",g1.retime/10);
    printf(1, "sleeping time:    %d \n",g1.stime/10);
    printf(1, "running time:     %d \n",g1.rutime/10);
    printf(1, "turnaround time:  %d \n\n",g1_turnaround/10);

    printf(1, "GROUP 2 - blocked:\n");
    printf(1, "waiting time:     %d \n",g2.retime/10);
    printf(1, "sleeping time:    %d \n",g2.stime/10);
    printf(1, "running time:     %d \n",g2.rutime/10);
    printf(1, "turnaround time:  %d \n\n",g2_turnaround/10);

    printf(1, "GROUP 3 - mixed:\n");
    printf(1, "waiting time:     %d \n",g3.retime/10);
    printf(1, "sleeping time:    %d \n",g3.stime/10);
    printf(1, "running time:     %d \n",g3.rutime/10);
    printf(1, "turnaround time:  %d \n\n",g3_turnaround/10);


    printf(1, "AVERAGE:\n");
    printf(1, "waiting time:     %d \n",average.retime/30);  // maybe changed to retime+stime
    printf(1, "sleeping time:    %d \n",average.stime/30);
    printf(1, "running time:     %d \n",average.rutime/30);
    printf(1, "turnaround time:  %d \n\n",avg_turnaround/30);

  exit(0);
}