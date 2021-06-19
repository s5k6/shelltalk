#define _POSIX_C_SOURCE 200809L

#include <unistd.h>
#include <err.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv) {

    if (argc < 2)
        errx(1, "Need command to run");

    warnx("Running command with crooked environment:");

    /* Don't do this, it's evil.  This is only a demonstration of
       possibilities!
    */
    char *env[] = {
        strndup("x=1", 4),
        strndup("x=2", 4),      // same name
        strndup("banana", 7),   // not `key=value` format
        NULL
    };

    for (int i = 0; env[i]; i++)
        printf("env[%d] = %s\n", i, env[i]);

    /* Shift the command line arguments one to the left, so the first
       argument becomes the program to run, the rest become its
       arguments.
    */
    execve(argv[1], argv + 1, env);


    err(1, "execve"); // reaching here indicates an error.

    return 0;
}
