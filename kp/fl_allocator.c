#include "fl_allocator.h"
#include "mem_common.h"
#include "fl_block.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdint.h>

static void *fl_heap_start = NULL;
static size_t fl_region_size = 0;

/* Выделяет регион памяти и инициализирует первый блок аллокатора */
static void* fl_alloc_region(void *addr, size_t query) {
    size_t req = query > REGION_MIN_SIZE ? query : REGION_MIN_SIZE;
    req = round_pages(req);
    void *region = map_pages(addr, req, 0);
    if (region == MAP_FAILED)
        return NULL;
    struct fl_block_header *block = region;
    block->next = NULL;
    block->capacity = req - FL_HEADER_SIZE;
    block->is_free = true;
    return region;
}

/* Ищем подходящий блок */
static struct fl_block_header* fl_find_block(struct fl_block_header *start, size_t query) {
    struct fl_block_header *current = start;
    while (current) {
        if (current->is_free && current->capacity >= query) {
            fl_split_block(current, query);
            current->is_free = false;
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/* Расширяет кучу, выделяя новый регион сразу за последним блоком */
static struct fl_block_header* fl_grow_heap(struct fl_block_header *last, size_t query) {
    struct fl_block_header *new_block = fl_alloc_region((uint8_t *)last + FL_HEADER_SIZE + last->capacity, query);
    if (new_block == NULL) return NULL;
    last->next = new_block;
    new_block->is_free = false;
    return new_block;
}

void fl_allocator_init(size_t initial_size) {
    fl_heap_start = fl_alloc_region(NULL, initial_size);
    if (!fl_heap_start) {
        fprintf(stderr, "Ошибка инициализации free-list аллокатора\n");
        exit(EXIT_FAILURE);
    }
    fl_region_size = round_pages(initial_size > REGION_MIN_SIZE ? initial_size : REGION_MIN_SIZE);
}

void fl_allocator_destroy() {
    if (fl_heap_start) {
        munmap(fl_heap_start, fl_region_size);
        fl_heap_start = NULL;
    }
}

void* fl_malloc(size_t size) {
    if (!fl_heap_start) {
        fprintf(stderr, "Free-list аллокатор не инициализирован\n");
        return NULL;
    }
    struct fl_block_header *start = (struct fl_block_header *)fl_heap_start;
    struct fl_block_header *block = fl_find_block(start, size);
    if (!block) {
        struct fl_block_header *current = start;
        while (current->next)
            current = current->next;
        block = fl_grow_heap(current, size);
    }
    return block ? block->contents : NULL;
}

void fl_free(void *ptr) {
    if (!ptr) return;
    struct fl_block_header *block = (struct fl_block_header *)((uint8_t*)ptr - FL_HEADER_SIZE);
    block->is_free = true;
    while (fl_try_merge_with_next(block));
}