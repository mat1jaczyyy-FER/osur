/*! Demo program */

#include <stdio.h>
#include <malloc.h>

void* a[15];

void print_memory() {
	for (int i = 0; i < 15; i++) {
		printf("[demo] a[%d] = %x %d\n", i, a[i], a[i]);
	}
	printf("\n");
}

int demo()
{
	printf("Hello Demo!\n");

	printf("[demo] Allocating 15 chunks\n");
	for (int i = 0; i < 15; i++) {
		a[i] = malloc(100);
	}
	print_memory();

	printf("[demo] Freeing first 4 chunks\n");
	for (int i = 0; i < 4; i++) {
		free(a[i]);
		a[i] = NULL;
	}
	print_memory();

	printf("[demo] Allocating large chunk\n");
	a[1] = malloc(300);
	print_memory();

	printf("[demo] Freeing leftover chunks\n");
	for (int i = 4; i < 15; i++) {
		free(a[i]);
		a[i] = NULL;
	}
	print_memory();

	printf("[demo] Freeing large chunk\n");
	free(a[1]);
	a[1] = NULL;
	print_memory();

	return 0;
}
