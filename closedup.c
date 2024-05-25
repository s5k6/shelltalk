#include <fcntl.h>
#include <err.h>
#include <unistd.h>


int main(int argc, char **argv) {

    (void)argc; (void)argv;

    int fd = open("closedup.out", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0)
        err(1, "open");

    if (dup2(fd, 1) < 0)
        err(1, "dup");

    if (write(1,  "write 1\n", 8) < 0) warn("write 1 to %d", 1);
    if (write(fd, "write 2\n", 8) < 0) warn("write 2 to %d", fd);

    warnx("Closing %d", fd);
    close(fd);

    if (write(1,  "write 3\n", 8) < 0) warn("write 3 to %d", 1);
    if (write(fd, "write 4\n", 8) < 0) warn("write 4 to %d", fd);

    warnx("Closing %d", 1);
    close(1);

    if (write(1,  "write 5\n", 8) < 0) warn("write 5 to %d", 1);
    if (write(fd, "write 6\n", 8) < 0) warn("write 6 to %d", fd);

    return 0;
}
