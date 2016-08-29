#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"
#include "signal.h"

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;

extern void inc_ticks(void);
//3.4
extern void* inject_sigreturn_start;
extern void* inject_sigreturn_end;
void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);
  
  initlock(&tickslock, "time");
}

void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}

//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
  if(tf->trapno == T_SYSCALL){
    if(proc->killed)
      exit(0); //@
    proc->tf = tf;
    syscall();
    if(proc->killed)
      exit(0); //@
    return;
  }

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(cpu->id == 0){
      acquire(&tickslock);
      ticks++;
      inc_ticks();
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpu->id, tf->cs, tf->eip);
    lapiceoi();
    break;
   
  //PAGEBREAK: 13
  default:
    if(proc == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpu->id, tf->eip, rcr2());
      panic("trap");
    }
    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x--kill proc\n",
            proc->pid, proc->name, tf->trapno, tf->err, cpu->id, tf->eip, 
            rcr2());
    proc->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running 
  // until it gets to the regular system call return.)
  if(proc && proc->killed && (tf->cs&3) == DPL_USER)
    exit(0); //@

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(proc && proc->state == RUNNING && tf->trapno == T_IRQ0+IRQ_TIMER)
    yield();

  // Check if the process has been killed since we yielded
  if(proc && proc->killed && (tf->cs&3) == DPL_USER)
    exit(0); //@
}

int get_tick(void){
  return (int)ticks; 
}

void apply_sig_handler(struct trapframe *tf){    
  if(proc == 0)  //scheduler
		return;
	if(proc->pending == 0) // no signals
		return;
	
  cprintf("apply_sig_handler: proc->pid = %d\n", proc->pid);  //debug print
  cprintf("   tf==proc->tf?   %d\n", proc->tf==tf);           //debug print


  int i;
	for(i=0; i<NUMSIG; i++){
  	if(IS_SIG_ON(proc,i)){
    	//backup the trapframe
      uint sp = proc->tf->esp;
  		memmove(proc->tfbackup,&tf,sizeof(struct trapframe));
  		TURN_OFF(proc,i);
    	tf->eip = (uint)(proc->sig_table[i]);
  		int length = (int)(&inject_sigreturn_end) - (int)(&inject_sigreturn_start);

  		sp -= length;
  		uint ret_address = sp;
  		copyout(proc->pgdir, sp, &inject_sigreturn_start, length);      	

    	sp -= 4;
    	*((uint*)sp) = i;
    	sp -= 4;
    	*((uint*)sp) = ret_address;

      proc->tf->esp = sp;
    	break;
    }
  }
  cprintf("apply_sig_handler - end\n");  //debug print
}