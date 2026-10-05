# Lab 2 notes: Augustin Carazanu, FAF-243

## Part 2. Use xv6 as the Unix it is

**(1) List three programs xv6 ships with.**
`cat`, `echo` and `grep`. Others include `ls`, `wc`, `mkdir`, `rm`, `kill`, `sh` and `usertests`.

**(2) Which two OS features must exist for a pipe to work?**
- **Process creation (`fork` and `exec`).** The shell must start `ls` and `grep` as two separate processes that run at the same time.
- **Inter-process communication (the `pipe` system call).** The kernel creates a buffer with a write end and a read end, which are file descriptors. `ls` writes its output into one end, and `grep` reads it as input from the other. The shell uses `close` and `dup` to connect them to standard output and standard input.

**(3) How does the xv6 shell compare to the Linux shell from Lab 1?**
It supports the same basic Unix syntax (commands with arguments, pipes `|`, redirection `<` `>` and `&`). However, it is much simpler than Linux bash: it has no command history, tab completion, variables or job control, and its prompt is only `$`.

## Part 3. Read the source

**(1) Which system calls does `user/cat.c` use, and what does each one ask the kernel to do?**
- `read(fd, buf, sizeof(buf))` asks the kernel to copy up to 512 bytes from the open file `fd` into the program's buffer. It returns the number of bytes read, 0 at the end of the file, or a negative value on error.
- `write(1, buf, n)` asks the kernel to write `n` bytes from the buffer to file descriptor 1 (standard output, the console).
- `open(path, O_RDONLY)` asks the kernel to open a file by name and return a new file descriptor. `close(fd)` releases that descriptor. Both are used in `main` for each file given as an argument.
- `exit(status)` asks the kernel to end the process and free its resources.

`fprintf(2, ...)` is not a system call. It is a library function (`user/printf.c`) that formats the text and then calls `write` on file descriptor 2 (standard error).

**(2) In which kernel file, and on which line, is `sys_read` implemented?**
In `kernel/sysfile.c`, on line 69 (`sys_read(void)`). `sys_write` is on line 83 of the same file.

**(3) What is the difference between the code in `kernel/` and the code in `user/`?**
The code in `kernel/` is the operating system itself. It runs in privileged supervisor mode with full access to the hardware and memory. The code in `user/` runs in unprivileged user mode, and it can only get services such as files, memory and devices by asking the kernel through system calls.

`user/user.h` is the full list of these requests: `fork`, `exit`, `wait`, `pipe`, `read`, `write`, `open`, `close`, `exec`, `pause` and the others.

## Note

The current xv6-riscv renamed the `sleep` system call to `pause` (see `user/user.h`), so `user/sleep.c` calls `pause(atoi(argv[1]))`.
