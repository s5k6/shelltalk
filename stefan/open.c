#include <err.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {

    int fd = open("output", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0)
        err(1, "open");

    ssize_t n = write(fd, "Hello named file\n", 17);
    if (n != 17)
        err(1, "write");

    if (close(fd) != 0)
        err(1, "close");

    return 0;
}
