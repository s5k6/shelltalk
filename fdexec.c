#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define FILENAME "fdexec.out"


int main(void) {


    /* Open a file for writing */

    int fd = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0)
        err(1, "open");

    warnx("open(\"" FILENAME "\") returned %d", fd);


    /* Replace program with `./write`, which writes to the FD passed
       as argument. For conversion to string, see snprintf(3) */

    char buf[8]; // expected range: 0 <= value < 1024
    snprintf(buf, 8, "%d", fd);

    execl("./write", "./write", buf, NULL);
    err(1, "execve");


    return 0;
}
