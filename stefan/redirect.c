#include <err.h>
#include <unistd.h>
#include <fcntl.h>

extern char **environ;

int main(int argc, char **argv) {

    if (argc < 2)
        errx(1, "Need command to run");

    pid_t pid = fork();
    if (pid < 0)
        err(1, "fork");

    if (pid == 0) {

        // open file to write to
        int fd = open("output", O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd < 0)
            err(1, "open");

        // make 1 an alias of the new file descriptor
        if (dup2(fd, 1) < 0)
            err(1, "dup2");

        // replace child with the program to run
        execve(argv[1], argv + 1, environ);
        err(1, "execve");
    }

    // this is the parent process
    write(1, "parent stdout still working\n", 28);

    return 0;
}
