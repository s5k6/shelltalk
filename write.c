#include <unistd.h>
#include <err.h>

int main(void) {

    int fd = 5;

    ssize_t n;


    /* Just write to FD 5, without opening it first
    */
    n = write(fd, "play the kazoo\n", 15);

    if (n < 0)
      err(1, "write to file descriptor %d failed", fd);

    return 0;
}
