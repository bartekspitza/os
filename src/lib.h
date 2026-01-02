#pragma once
#include <stdint.h>

int atoi(const char* str);
char* itoa(int value, unsigned int base, char* buf, size_t buf_size);

void *memset(void *dst, int fill_with, size_t n_bytes);
void *memcpy(void *dst, const void *src, size_t n_bytes);