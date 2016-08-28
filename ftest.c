// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#include "types.h"
#include "stat.h"
#include "user.h"

#define N  1000

void
forktest(void)
{

  printf(1, "f_test\n");


fork();
fork();
fork();
fork();

  for(;;){

  }
}

  

int
main(void)
{
  forktest();
  exit(0);
}
