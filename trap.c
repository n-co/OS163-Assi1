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
  //backup the trapframe here
  proc->tfbackup->edi = tf->edi;
  proc->tfbackup->esi = tf->esi;
  proc->tfbackup->ebp = tf->ebp;
  proc->tfbackup->oesp = tf->oesp;     
  proc->tfbackup->ebx = tf->ebx;
  proc->tfbackup->edx = tf->edx;
  proc->tfbackup->ecx = tf->ecx;
  proc->tfbackup->eax = tf->eax;
  proc->tfbackup->gs = tf->gs;
  proc->tfbackup->padding1 = tf->padding1;
  proc->tfbackup->fs = tf->fs;
  proc->tfbackup->padding2 = tf->padding2;
  proc->tfbackup->es = tf->es;
  proc->tfbackup->padding3 = tf->padding3;
  proc->tfbackup->ds = tf->ds;
  proc->tfbackup->padding4 = tf->padding4;
  proc->tfbackup->trapno = tf->trapno;
  proc->tfbackup->err = tf->err;
  proc->tfbackup->eip = tf->eip;
  proc->tfbackup->cs = tf->cs;
  proc->tfbackup->padding5 = tf->padding5;
  proc->tfbackup->eflags = tf->eflags;
  proc->tfbackup->esp = tf->esp;
  proc->tfbackup->ss = tf->ss;
  proc->tfbackup->padding6 = tf->padding6;


  int i;
  for(i=0; i<NUMSIG; i++){
    if(IS_SIG_ON(proc,i)){
      // cancel signal - off
      //ip=func of sig handler
      // change end of function to sigreturn
      (proc->sig_table[i])();
      break;
    }
  }
}