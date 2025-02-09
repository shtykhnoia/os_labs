// buddy_allocator.c
#include "buddy_allocator.h"
#include "mem_common.h"
#include "buddy_block.h"
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define REGION_MIN_SIZE (1 << 20)  // Минимальный размер региона: 1 МБ

static void *buddy_region = NULL;
static size_t buddy_region_size = 0;
static unsigned int buddy_max_level = 0;
// free_lists[i] содержит только свободные блоки для уровня i
static struct buddy_block_header **free_lists = NULL;

// Инициализация массива free_lists с использованием mmap
static void init_free_lists(unsigned int levels) {
    size_t array_size = (levels + 1) * sizeof(struct buddy_block_header *);
    size_t alloc_size = round_pages(array_size);
    free_lists = map_pages(NULL, alloc_size, 0);
    if (free_lists == MAP_FAILED) {
        perror("mmap free_lists");
        exit(EXIT_FAILURE);
    }
    memset(free_lists, 0, alloc_size);
}

void buddy_allocator_init(size_t initial_size) {
    size_t req = initial_size > REGION_MIN_SIZE ? initial_size : REGION_MIN_SIZE;
    buddy_region_size = round_pages(req);
    buddy_region = map_pages(NULL, buddy_region_size, 0);
    if (buddy_region == MAP_FAILED) {
        perror("mmap buddy_region");
        exit(EXIT_FAILURE);
    }
    // Определение максимального уровня: MIN_BUDDY_SIZE * 2^(buddy_max_level) >= buddy_region_size
    size_t max_block = buddy_region_size;
    buddy_max_level = 0;
    while (MIN_BUDDY_SIZE * (1UL << buddy_max_level) < max_block)
        buddy_max_level++;

    init_free_lists(buddy_max_level);

    // Инициализация всего региона как одного свободного блока
    struct buddy_block_header *initial = buddy_region;
    initial->level = buddy_max_level;
    initial->is_free = true;
    initial->next = initial->prev = NULL;
    free_lists[buddy_max_level] = initial;
}

void* buddy_malloc(size_t size) {
    if (size == 0)
        return NULL;

    // Учитываем пространство для заголовка блока
    size_t total_size = size + sizeof(struct buddy_block_header);
    unsigned int req_level = 0;
    size_t req = MIN_BUDDY_SIZE;
    while (req < total_size) {
        req *= 2;
        req_level++;
    }
    // Поиск подходящего свободного блока (free_list содержит только свободные блоки)
    for (unsigned int level = req_level; level <= buddy_max_level; level++) {
        if (free_lists[level]) {
            while (level > req_level) {
                bb_split_block(level, free_lists);
                level--;
            }
            struct buddy_block_header *block = free_lists[req_level];
            bb_remove_free_block(&free_lists[req_level], block);
            block->is_free = false;
            return (void *)((uint8_t *)block + sizeof(struct buddy_block_header));
        }
    }
    return NULL;
}

void buddy_free(void* ptr) {
    if (!ptr)
        return;
    struct buddy_block_header *block = (struct buddy_block_header *)((uint8_t *)ptr - sizeof(struct buddy_block_header));
    block->is_free = true;
    bb_insert_free_block(&free_lists[block->level], block);
    bb_merge_block(free_lists, buddy_max_level, buddy_region, block);
}

void buddy_allocator_destroy() {
    if (buddy_region)
        munmap(buddy_region, buddy_region_size);
    buddy_region = NULL;
    buddy_region_size = 0;
    if (free_lists) {
        size_t array_size = (buddy_max_level + 1) * sizeof(struct buddy_block_header *);
        size_t alloc_size = round_pages(array_size);
        munmap(free_lists, alloc_size);
        free_lists = NULL;
    }
}