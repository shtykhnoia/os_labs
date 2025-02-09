#include "mem_common.h"
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>

// Округление размера до ближайшего большего кратного размеру страницы
size_t round_pages(size_t size) {
    size_t pagesize = sysconf(_SC_PAGESIZE);
    return ((size + pagesize - 1) / pagesize) * pagesize;
}

// Функция для выделения памяти через mmap с режимом чтения и записи
void* map_pages(void* addr, size_t length, int prot) {
    // Игнорируем параметр prot и используем PROT_READ | PROT_WRITE, а также флаги MAP_PRIVATE | MAP_ANONYMOUS
    void *mapped = mmap(addr, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap failed");
    }
    return mapped;
}