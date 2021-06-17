#include <unistd.h>
#include <err.h>

int main(void) {

    ssize_t n;


    /* Just write to FD 5, without opening it first
    */
    n = write(5, "play the kazoo\n", 15);

    if (n < 0)
      err(1, "write to file descriptor failed");

    return 0;
}
