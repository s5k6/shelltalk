#include <unistd.h>
#include <err.h>

extern char **environ;

int main(int argc, char **argv) {

    if (argc < 2)
        errx(1, "Need command to run");

    warnx("PID %u, about to run %s", getpid(), argv[1]);

    execve(argv[1], argv + 1, environ);

    err(1, "execve"); // reaching here indicates an error.

    return 0;
}
