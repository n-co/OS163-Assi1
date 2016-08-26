#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
int pid;
int status;

if (!(pid = fork()))
{
exit(0x7f);
}
else
{
wait(&status);
}
if (status == 0x7f)
{
printf(1, "OK\n");
}
else
{
printf(1, "FAILED\n");
}
return 0;
//exit(0);
}
