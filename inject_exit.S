#include "syscall.h"
#include "traps.h"

.text
	.global inject_exit_start
	.global inject_exit_end

	inject_exit_start:
			pushl %eax
			movl $SYS_exit, %eax
			int $T_SYSCALL
	inject_exit_end:
	ret		
