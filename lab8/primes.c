#include "kernel/types.h"
#include "user/user.h"

// One stage of the sieve: read numbers from fd. The first one is a prime;
// pass on only the numbers it does not divide, to the next stage.
void
sieve(int fd) __attribute__((noreturn));

void
sieve(int fd)
{
  int p, n;

  if(read(fd, &p, sizeof(p)) != sizeof(p)){   // no more numbers: done
    close(fd);
    exit(0);
  }
  printf("prime %d\n", p);

  int next[2];
  pipe(next);
  if(fork() == 0){                      // child: the next stage
    close(next[1]);
    close(fd);
    sieve(next[0]);
  }
  close(next[0]);                       // this stage only writes
  while(read(fd, &n, sizeof(n)) == sizeof(n))
    if(n % p != 0)
      write(next[1], &n, sizeof(n));
  close(fd);
  close(next[1]);                       // lets the next stage see EOF
  wait(0);
  exit(0);
}

int
main(int argc, char *argv[])
{
  int p[2];

  pipe(p);
  if(fork() == 0){
    close(p[1]);
    sieve(p[0]);
  }
  close(p[0]);
  for(int i = 2; i <= 35; i++)
    write(p[1], &i, sizeof(i));
  close(p[1]);
  wait(0);
  exit(0);
}
