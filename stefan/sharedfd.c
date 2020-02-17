#include <err.h>
#include <fcntl.h>
#include <unistd.h>


int main(void) {

    int fd = open("output", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0)
        err(1, "open");

    pid_t pid = fork();
    if (pid < 0)
        err(1, "fork");

    if (pid == 0)
        // child process
        write(fd, "I am the child\n", 15);

    else
        // parent process
        write(fd, "I am the parent\n", 16);

    return 0;
}
