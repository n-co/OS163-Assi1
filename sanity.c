#include "types.h"
#include "stat.h"
#include "perf.h"
#include "user.h"
#include "syscall.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
  static char digits[] = "0123456789ABCDEF";
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';

  while(--i >= 0)
    putc(fd, buf[i]);
}

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, char *fmt, ...)
{
  char *s;
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
        state = '%';
      } else {
        putc(fd, c);
      }
    } else if(state == '%'){
      if(c == 'd'){
        printint(fd, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
        printint(fd, *ap, 16, 0);
        ap++;
      } else if(c == 's'){
        s = (char*)*ap;
        ap++;
        if(s == 0)
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
        }
      } else if(c == 'c'){
        putc(fd, *ap);
        ap++;
      } else if(c == '%'){
        putc(fd, c);
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
    }
  }
}


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
  int start = uptime();
  while(uptime()<start+30)
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
