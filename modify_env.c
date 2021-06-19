#include <unistd.h>
#include <err.h>
#include <string.h>

extern char **environ;

int main(int argc, char **argv) {

    (void)argc; (void)argv;

    warnx("PID %u", getpid());
    warnx("environ[0] = %s", environ[0]);

    /* Don't do this, it's evil.  This is only a demonstration of
       possibilities!
    */
    strncpy(environ[0], "hello world", 12);

    warnx("environ[0] = %s", environ[0]);

    warnx("Sleeping 100s");
    sleep(100);

    /*
    While sleeping, use

        $ tr '\0' '\n' < /proc/9242/environ | hexdump -C | head

    to see the effect on the process' environment.
    */

    return 0;
}
