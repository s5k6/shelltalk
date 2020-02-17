#include <unistd.h>


int main(void) {

    // write to 1
    write(1, "knock knock\n", 12);

    // close 1, and make 1 an alias of 2
    dup2(2, 1);

    // write to 1 as above!
    write(1, "who's there\n", 12);

    return 0;
}
