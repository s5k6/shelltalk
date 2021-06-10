#include <stdlib.h>
#include <unistd.h>
#include <err.h>

int main(int argc, char **argv) {
    (void)argv;


    /* Print this process' PID, and the one of its parent.
    */
    warnx("PID %u, PPID %u", getpid(), getppid());


    /* If an argument exists, sleep 5 seconds.
    */
    if (argc > 1)
        sleep(5);


    return 0;
}
