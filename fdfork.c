#include <err.h>
#include <fcntl.h>
#include <unistd.h>


int main(void) {

    warnx("Writing to file: output");


    // Open a file for writing
    int fd = open("output", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0)
        err(1, "open");


    // Fork a child
    pid_t pid = fork();
    if (pid < 0)
        err(1, "fork");


    if (pid == 0) {

        // child process
        write(fd, "I am the child\n", 15);

    } else {

        // parent process
        write(fd, "I am the parent\n", 16);

    }

    // both processes again
    close(fd);

    return 0;
}
