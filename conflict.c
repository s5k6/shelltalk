#include <fcntl.h>
#include <err.h>
#include <unistd.h>


int main(int argc, char **argv) {

    (void)argc; (void)argv;

    int fd1 = open("conflict.out", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd1 < 0)
        err(1, "first open");

    int fd2 = open("conflict.out", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd2 < 0)
        err(1, "second open");

    write(fd1, "Sixteen men on a dead man's chest\n", 34);

    write(fd2, "and a bottle of rum", 20);

    return 0;
}
