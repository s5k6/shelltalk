#include <unistd.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char **argv) {

    if (argc < 3)
        errx(1, "Need two programs to connect.");

    /* Creates a unidirectional data channel.  pipefd[0] refers to the
       read end of the pipe.  pipefd[1] refers to the write end.
    */
    int pipefd[2];
    if (pipe(pipefd) != 0)
        err(1, "pipe failed");

    pid_t pid;


    /* Fork a child process for the writer. */

    pid = fork();
    if (pid < 0)
        err(1, "fork");

    if (pid == 0) { // this is the child

        dup2(pipefd[1], 1);  // set FD 1 to write end of pipe, replacing stdout

        close(pipefd[0]);  // close unused descriptORS
        close(pipefd[1]);

        execlp(argv[1], argv[1], (char *)NULL);  // replace by writer program
        err(1, "exec %s", argv[1]);

    }


    /* this is the parent */

    warnx("%d runs %s", pid, argv[1]);


    /* Fork a child process for the reader. */

    pid = fork();
    if (pid < 0)
        err(1, "fork");

    if (pid == 0) { // this is the child

        dup2(pipefd[0], 0);  // det FD 0 to read end of pipe, replacing stdin

        close(pipefd[0]);  // close unused descriptORS
        close(pipefd[1]);

        execlp(argv[2], argv[2], (char *)NULL);  // replace by reader program
        err(1, "exec %s", argv[2]);

    }


    /* this is the parent */

    warnx("%d runs %s", pid, argv[2]);

    close(pipefd[0]);  // close unused descriptors
    close(pipefd[1]);


    /* wait for children, get exit status */

    int status;
    while ((pid = wait(&status)) != -1) {
        if (WIFEXITED(status))
            warnx("Child %d returned %d", pid, WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            warnx("Child %d caught %d", pid, WTERMSIG(status));
        else
            warnx("Dunno why child %d terminated", pid);
    };
    if (errno != ECHILD) // no unwaited-for children
        err(1, "wait");

    return 0;
}
