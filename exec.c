#include <unistd.h>
#include <err.h>

extern char **environ;

int main(int argc, char **argv) {

    if (argc < 2)
        errx(1, "Need command to run");

    warnx("PID %u, about to run %s", getpid(), argv[1]);


    /* Shift the command line arguments one to the left, so the first
       argument becomes the program to run, the rest become its
       arguments.
    */
    execve(argv[1], argv + 1, environ);


    err(1, "execve"); // reaching here indicates an error.

    return 0;
}
