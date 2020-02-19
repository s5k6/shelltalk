#include <fcntl.h>
#include <err.h>
#include <unistd.h>


int main(int argc, char **argv) {

    (void)argc; (void)argv;

    int fd = open("output", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0)
        err(1, "open");

    if (dup2(fd, 1) < 0)
        err(1, "dup");

    if (write(1,  "write 1\n", 8) < 0) warn("1");
    if (write(fd, "write 2\n", 8) < 0) warn("2");

    close(fd);
    if (write(1,  "write 3\n", 8) < 0) warn("3");
    if (write(fd, "write 4\n", 8) < 0) warn("4");

    close(1);
    if (write(1,  "write 5\n", 8) < 0) warn("5");
    if (write(fd, "write 6\n", 8) < 0) warn("6");

    return 0;
}
