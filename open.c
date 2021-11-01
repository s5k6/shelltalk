#include <err.h>
#include <fcntl.h>
#include <unistd.h>

#define FILENAME "open.out"


int main(void) {


    /* Open file named `output` for writing, create a new one, overwrite
       an existing one.
    */
    int fd = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0)
        err(1, "open");


    /* Use the file descriptor for writing. */
    ssize_t n = write(fd, "Hello named file\n", 17);
    if (n != 17)
        err(1, "write");


    if (close(fd) != 0)
        err(1, "close");

    return 0;
}
