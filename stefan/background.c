#include <unistd.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

int main(int argc, char **argv) {

    if (argc < 2)
        errx(1, "Need command to run");

    warnx("PID %u", getpid());

    pid_t pid = fork();

    if (pid < 0)
        err(1, "fork");

    if (pid == 0) {
        execve(argv[1], argv + 1, environ);
        err(1, "execve");
    }

    // pid is >0 now!
    warnx("My child process has ID %u", pid);


#if 0
    int status;
    pid = wait(&status);
    if (pid < 0)
        err(1, "wait");

    if (WIFEXITED(status))
        warnx("Child %u has returned %d", pid, WEXITSTATUS(status));
    else
        warnx("Other status change on %u", pid);
#endif

    return 0;
}
