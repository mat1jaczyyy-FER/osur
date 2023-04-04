#include <types/basic.h>

void premjesti_blok(char* from, char* to, size_t size)
{
	size_t i;
	for (i = 0; i < size; i++)
		to[i] = from[i];
}

void premjesti() {
    extern char size_bootcode, size_code, size_rodata, size_data, size_bss;
    
    size_t start = LOAD_ADDR;
    start += (size_t)&size_bootcode;

    premjesti_blok((char*)start, (char*)CODE_ADDR, (size_t)&size_code);
    start += (size_t)&size_code;

    premjesti_blok((char*)start, (char*)RODATA_ADDR, (size_t)&size_rodata);
    start += (size_t)&size_rodata;

    premjesti_blok((char*)start, (char*)DATA_ADDR, (size_t)&size_data);
    start += (size_t)&size_data;

    premjesti_blok((char*)start, (char*)(DATA_ADDR + (size_t)&size_data), (size_t)&size_bss);
}