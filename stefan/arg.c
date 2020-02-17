#include <stdio.h>

int main(int argc, char **argv) {

    for (int i = 0; i < argc; i++)
        if (printf("%6d: %s\n", i, argv[i]) < 0)
            return 1;

    return 0;
}
