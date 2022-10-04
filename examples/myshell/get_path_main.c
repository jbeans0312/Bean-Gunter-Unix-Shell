#include "get_path.h"

main()
{
  char cmd[64];
  struct pathelement *p;

  p = get_path();
  while (p) {         // WHERE
    sprintf(cmd, "%s/gcc", p->element); //stores the string p->element/gcc onto the buffer cmd
    if (access(cmd, F_OK) == 0) //checks if the cmd is accessible using the flag "F_OK" 
				//returns 0 if the file is accessible, returns -1 otherwise
					//"F_OK" flag checks if the file exists in the given directory (cmd in this case)
      printf("[%s]\n", cmd);
    p = p->next;
  }

  printf("----------\n");

  p = get_path();
  while (p) {         // WHICH
    sprintf(cmd, "%s/gcc", p->element); //stores the string p->element/gcc onto the buffer cmd
    if (access(cmd, X_OK) == 0) { //checks if the cmd file has executible perms using the flag "X_OK"
				  	//"X_OK" flag checks if the file has the executible/write perms in the given directory (cmd in this case
      printf("[%s]\n", cmd);
      break;
    }
    p = p->next;
  }
}
