#include <unistd.h>
#include <err.h>
#include <sys/types.h>


/* WARNING this is a minimal working example, not best practice!

Reading and writing should always be done in loops, checking the
number of bytes actually read or written, and adjusting accordingly!
*/

int main(void) {

    /* Creates a unidirectional data channel.  pipefd[0] refers to the
       read end of the pipe.  pipefd[1] refers to the write end.
    */
    int pipefd[2];
    if (pipe(pipefd) != 0)
        err(1, "pipe failed");

    /* Fork a child process. */
    pid_t pid = fork();
    if (pid < 0)
        err(1, "fork");

    if (pid == 0) { // this is the child

        close(pipefd[0]);  // the child will not read

        // write to pipe, see warning above
        if (write(pipefd[1], "Hello child process\n", 20) != 20)
            errx(1, "Written unexpected number of bytes");

        close(pipefd[1]);

    } else { // this is the parent

        close(pipefd[1]);  // the parent will not write

        // read from pipe, see warning above
        char buf[20];
        if (read(pipefd[0], buf, 20) != 20)
            err(1, "Read unexpected number of bytes");

        // print received data
        write(1, buf, 20);

        close(pipefd[0]);
    }

    return 0;
}
