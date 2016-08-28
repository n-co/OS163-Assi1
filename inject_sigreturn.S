#include "syscall.h"
#include "traps.h"

.text
	.global inject_sigreturn_start
	.global inject_sigreturn_end

	inject_sigreturn_start:
			pushl %eax
			movl $SYS_sigreturn, %eax
			int $T_SYSCALL
	inject_sigreturn_end:
