#include <unistd.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

int main(int argc, char **argv) {

    if (argc < 2)
        errx(1, "Need command to run");

    warnx("PID %u", getpid());


    /* Fork a child */
    pid_t pid = fork();
    if (pid < 0)
        err(1, "fork");


    /* If this is the child process, replace yourself with the command
       passed as arguments.
    */
    if (pid == 0) {
        execve(argv[1], argv + 1, environ);
        err(1, "execve");
    }

    /* pid is >0, otherwise we do not reach this point
    */
    warnx("My child process has ID %u", pid);


    // sleep(1);  // maybe sleep to observe PPID reported by child

    return 0;
}
