#include <unistd.h>
#include <err.h>
#include <sys/types.h>

int main(void) {

    warnx("before, PID %u, PPID %u", getpid(), getppid());

    pid_t pid = fork();
    if (pid < 0)
        err(1, "fork");

    warnx("after, PID %u, PPID %u, pid %u", getpid(), getppid(), pid);

    return 0;
}
