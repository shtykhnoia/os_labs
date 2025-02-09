#ifndef MEM_COMMON_H
#define MEM_COMMON_H

#include <unistd.h>
#include <sys/mman.h>
#include <stddef.h>

#define REGION_MIN_SIZE (2 * 4096)

/* Вычисляет количество страниц, необходимых для mem байт */
static inline size_t pages_count(size_t mem) {
    size_t ps = getpagesize();
    return mem / ps + ((mem % ps) > 0);
}

/* Округляет размер до ближайшего кратного размеру страницы */
static inline size_t round_pages(size_t mem) {
    size_t ps = getpagesize();
    return ps * pages_count(mem);
}

/* Завернутая функция для выделения памяти через mmap */
static inline void* map_pages(void *addr, size_t length, int additional_flags) {
    return mmap(addr, length, PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS | additional_flags, -1, 0);
}

#endif