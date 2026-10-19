#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int p2c[2], c2p[2];                   // parent-to-child, child-to-parent
  char b;

  pipe(p2c);
  pipe(c2p);

  if(fork() == 0){
    close(p2c[1]); close(c2p[0]);       // the ends the child does not use
    if(read(p2c[0], &b, 1) != 1){       // wait for the ping
      fprintf(2, "child: read failed\n");
      exit(1);
    }
    printf("%d: received ping\n", getpid());
    write(c2p[1], &b, 1);               // send the byte back
    close(p2c[0]); close(c2p[1]);
    exit(0);
  }

  close(p2c[0]); close(c2p[1]);         // the ends the parent does not use
  b = 'x';
  write(p2c[1], &b, 1);                 // ping
  if(read(c2p[0], &b, 1) != 1){         // blocks until the child replies
    fprintf(2, "parent: read failed\n");
    exit(1);
  }
  printf("%d: received pong\n", getpid());
  close(p2c[1]); close(c2p[0]);
  wait(0);
  exit(0);
}
