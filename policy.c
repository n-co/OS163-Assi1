#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"

int
main(int argc, char *argv[])
{
  if(argc<2){
    printf(1,"ERROR: wrong number of arguments\n");
    exit(0);
  }
  int pol=atoi(argv[1]);

  switch(pol){
    case POL_UNIFORM:  
    case POL_PRIORITY: 
    case POL_DYNAMIC: schedp(pol);
        break;
    default: printf(1,"ERROR: wrong policy id\n");
        break;
  }

  exit(0);
}
