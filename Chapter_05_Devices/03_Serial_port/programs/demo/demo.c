/*! Demo program */

#include <stdio.h>
#include <malloc.h>

void* a[100];

int demo()
{
	printf("Hello Demo!\n");

	printf("[demo] Allocating chunks\n");
	for (int i = 0; i < 100; i++) {
		a[i] = malloc(50000);
	}

	printf("[demo] Freeing every other chunk\n");
	for (int i = 1; i < 100; i += 2) {
		free(a[i]);
		a[i] = NULL;
	}

	float f = frag();
	printf("[demo] Fragmentation: %d.%d\n", (int)f, (int)((f - (int)f) * 1000000));

	return 0;
}
