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

        dup2(pipefd[1], 1);  // use write end for stdout

        close(pipefd[0]);  // close unused descriptors
        close(pipefd[1]);

        execlp(argv[1], argv[1], (char *)NULL);  // replace by writer
        err(1, "exec %s", argv[1]);

    }

    warnx("%d runs %s", pid, argv[1]);



    /* Fork a child process for the reader. */

    pid = fork();
    if (pid < 0)
        err(1, "fork");

    if (pid == 0) { // this is the child

        dup2(pipefd[0], 0);  // use read end for stdin

        close(pipefd[0]);  // close unused descriptors
        close(pipefd[1]);

        execlp(argv[2], argv[2], (char *)NULL);  // replace by reader
        err(1, "exec %s", argv[2]);

    }

    warnx("%d runs %s", pid, argv[2]);



    /* this is the parent */

    close(pipefd[0]);  // close unused descriptors
    close(pipefd[1]);

    // wait for children, get exit status
    int status;
    while ((pid = wait(&status)) != -1) {
        if (WIFEXITED(status))
            warnx("Child %d returned %d", pid, WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            warnx("Child %d caught %d", pid, WTERMSIG(status));
        else
            warnx("Child %d terminated", pid);
    };
    if (errno != ECHILD)
        err(1, "wait");

    return 0;
}
