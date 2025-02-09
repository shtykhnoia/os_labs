#ifndef MEM_COMMON_H
#define MEM_COMMON_H

#include <stddef.h>

// Функция для округления размера до размера страницы
size_t round_pages(size_t size);

// Функция для выделения области памяти с использованием mmap
void* map_pages(void* addr, size_t length, int prot);

#endif // MEM_COMMON_H