#include <unistd.h>
#include <err.h>
#include <sys/types.h>

int main(void) {

    warnx("before, PID %u, PPID %u", getpid(), getppid());


    /* Fork a child process.  Returns twice, in the parent, `pid` will
       be the child's PID, in the child it will be zero.
    */
    pid_t pid = fork();
    if (pid < 0)
        err(1, "fork");


    /* Note: There are two processes now, executing this code.
    */
    warnx("after,  PID %u, PPID %u, fork() returned %u",
          getpid(), getppid(), pid);


    return 0;
}
