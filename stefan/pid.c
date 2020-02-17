#include <stdlib.h>
#include <unistd.h>
#include <err.h>

int main(int argc, char **argv) {

    warnx("PID %u, PPID %u", getpid(), getppid());

    // if given as argument, wait some seconds
    if (argc > 1)
        sleep((unsigned int)atoi(argv[1]));

    return 0;
}
