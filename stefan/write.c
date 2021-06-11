#include <unistd.h>
#include <err.h>

int main(void) {

    ssize_t n;

    int fd = 5;

    n = write(fd, "play the kazoo\n", 15);

    if (n < 0)
      err(1, "write to file descriptor %i failed", fd);

    return 0;
}
