#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int show_inum = 0;   // set by the optional -n argument

// Return a pointer to the last component of path (after the last '/').
char *
basename(char *path)
{
  char *p;
  for(p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  return p + 1;
}

void
find(char *path, char *name)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  // Does this path itself match?
  if(strcmp(basename(path), name) == 0){
    if(show_inum)
      printf("%s %d\n", path, st.ino);
    else
      printf("%s\n", path);
  }

  if(st.type != T_DIR){
    close(fd);
    return;
  }

  if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
    fprintf(2, "find: path too long\n");
    close(fd);
    return;
  }
  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';

  // Read the directory entry by entry.
  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0)                       // free slot
      continue;
    if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
      continue;                            // never recurse into . and ..
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;
    find(buf, name);                       // recurse into the child
  }
  close(fd);                               // only 16 open files per process
}

int
main(int argc, char *argv[])
{
  if(argc < 3 || argc > 4 || (argc == 4 && strcmp(argv[3], "-n") != 0)){
    fprintf(2, "usage: find <dir> <name> [-n]\n");
    exit(1);
  }
  show_inum = (argc == 4);
  find(argv[1], argv[2]);
  exit(0);
}
