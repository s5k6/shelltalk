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


    /* Wait for a child to terminate.  Collect its PID and exit
       status.
    */
    int status;
    pid = wait(&status);
    if (pid < 0)
        err(1, "wait");


    /* Analyse and print exit status.
    */
    if (WIFEXITED(status))
        warnx("Child %u has returned %d", pid, WEXITSTATUS(status));
    else
        warnx("Other status change on %u", pid);


    return 0;
}
