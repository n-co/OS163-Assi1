#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"
#include "perf.h"

void defSigHandler(int signal){
	cprintf("A signal %d was accepted by process %d\n", signal, proc->pid);
}