#include <fcntl.h>
#include <err.h>
#include <unistd.h>

#define FILENAME "dup2.out"


int main(void) {


    // write to stdout, as usual
    write(1, "knock knock\n", 12);


    // open a file, gives some file descriptor
    int fd = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0)
        err(1, "open");

    warnx("open(\"" FILENAME "\") returned %d", fd);  // writes to stderr


    // close original stdout 1, and make it an alias of fd
    if (dup2(fd, 1) < 0)
        err(1, "dup");


    // write to 1 as above!
    write(1, "who's there\n", 12);


    return 0;
}
