#include <stdio.h>

void lab1z() {
    static int x = 0;
    printf("lab1z pokrenut %d puta\n", ++x);
}
