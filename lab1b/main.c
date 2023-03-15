#include <stdio.h>

extern void prvi();
extern void drugi();
extern void josjedandrugi();
extern void treci();

int main() {
    printf("print iz main.c!\n");

    prvi();
    drugi();
    josjedandrugi();
    treci();

    return 0;
}
