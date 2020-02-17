#include <stdlib.h>
#include <unistd.h>
#include <err.h>

int main(int argc, char **argv) {
    (void)argv;

    warnx("PID %u, PPID %u", getpid(), getppid());

    if (argc > 1)
        sleep(5);

    return 0;
}
