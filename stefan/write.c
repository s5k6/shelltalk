#include <unistd.h>
#include <err.h>

int main(void) {

    ssize_t n;

    n = write(2, "play the kazoo\n", 15);

    if (n < 0)
        err(1, "write failed");

    return 0;
}
