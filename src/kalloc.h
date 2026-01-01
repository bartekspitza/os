#pragma once

#define PAGE_SIZE 4096

void kinit(void);
void* kalloc();
void kfree(void* pa);