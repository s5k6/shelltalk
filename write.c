#include <unistd.h>
#include <err.h>
#include <stdlib.h>


int main(int argc, char **argv) {

    if (argc < 2)
        errx(1, "pass file descriptor as argument");


    /* Get file descriptor from first argument.  See strtol(3) for
       conversion. */

    char *end = argv[1];
    int fd = (int)strtol(argv[1], &end, 10);
    if (end == argv[1] || *end != '\0')
        errx(1, "Invalid argument");

    warnx("Using file descriptor %d.", fd);


    /* Just write to given file descriptor, without opening a file
       first. */

    ssize_t n = write(fd, "play the kazoo\n", 15);

    if (n < 15)
      err(1, "Write to file descriptor %d failed", fd);


    return 0;
}
