#include "buddy_allocator.h"
#include "mem_common.h"
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#define MIN_BUDDY_SIZE 32

/* Заголовок блока buddy аллокатора */
struct buddy_block_header {
    unsigned int level;  // размер блока = MIN_BUDDY_SIZE * 2^(level)
    bool is_free;
    struct buddy_block_header *next;
};

static void *buddy_region = NULL;
static size_t buddy_region_size = 0;
static unsigned int buddy_max_level = 0;
static struct buddy_block_header **free_lists = NULL;

/* Инициализируем массив списков свободных блоков */
static void init_free_lists(unsigned int levels) {
    free_lists = malloc((levels + 1) * sizeof(struct buddy_block_header*));
    if (!free_lists) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    for (unsigned int i = 0; i <= levels; i++)
        free_lists[i] = NULL;
}

void buddy_allocator_init(size_t initial_size) {
    size_t req = initial_size > REGION_MIN_SIZE ? initial_size : REGION_MIN_SIZE;
    buddy_region_size = round_pages(req);
    buddy_region = map_pages(NULL, buddy_region_size, 0);
    if (buddy_region == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    size_t max_block = buddy_region_size;
    buddy_max_level = 0;
    while (MIN_BUDDY_SIZE * (1UL << buddy_max_level) < max_block)
        buddy_max_level++;
    init_free_lists(buddy_max_level);
    
    struct buddy_block_header *initial = buddy_region;
    initial->level = buddy_max_level;
    initial->is_free = true;
    initial->next = NULL;
    free_lists[buddy_max_level] = initial;
}

static void split_block(unsigned int level) {
    if (level == 0 || free_lists[level] == NULL)
        return;
    struct buddy_block_header *block = free_lists[level];
    free_lists[level] = block->next;
    unsigned int new_level = level - 1;
    size_t block_size = MIN_BUDDY_SIZE * (1UL << level);
    size_t half_size = block_size / 2;
    
    struct buddy_block_header *first = block;
    first->level = new_level;
    first->is_free = true;
    first->next = NULL;
    
    struct buddy_block_header *second = (struct buddy_block_header *)((uint8_t*)block + half_size);
    second->level = new_level;
    second->is_free = true;
    second->next = NULL;
    
    first->next = free_lists[new_level];
    free_lists[new_level] = first;
    
    second->next = free_lists[new_level];
    free_lists[new_level] = second;
}

static unsigned int level_for_size(size_t size) {
    size_t req = MIN_BUDDY_SIZE;
    unsigned int level = 0;
    while (req < size) {
        req *= 2;
        level++;
    }
    return level;
}

static struct buddy_block_header* get_block(unsigned int req_level) {
    for (unsigned int level = req_level; level <= buddy_max_level; level++) {
        if (free_lists[level]) {
            while(level > req_level) {
                split_block(level);
                level--;
            }
            struct buddy_block_header *block = free_lists[req_level];
            free_lists[req_level] = block->next;
            block->is_free = false;
            block->next = NULL;
            return block;
        }
    }
    return NULL;
}

void* buddy_malloc(size_t size) {
    if (size == 0)
        return NULL;
    size_t total_size = size + sizeof(struct buddy_block_header);
    unsigned int req_level = level_for_size(total_size);
    struct buddy_block_header *block = get_block(req_level);
    if (!block)
        return NULL;
    return (void*)((uint8_t*)block + sizeof(struct buddy_block_header));
}

static struct buddy_block_header* buddy_of(struct buddy_block_header *block, unsigned int level) {
    size_t block_size = MIN_BUDDY_SIZE * (1UL << level);
    uintptr_t offset = (uintptr_t)block - (uintptr_t)buddy_region;
    uintptr_t buddy_offset = offset ^ block_size;
    return (struct buddy_block_header*)((uint8_t*)buddy_region + buddy_offset);
}

static void merge_block(struct buddy_block_header *block) {
    while (block->level < buddy_max_level) {
        struct buddy_block_header *buddy = buddy_of(block, block->level);
        struct buddy_block_header *prev = NULL, *curr = free_lists[block->level];
        bool found = false;
        while (curr) {
            if (curr == buddy) {
                found = true;
                break;
            }
            prev = curr;
            curr = curr->next;
        }
        if (!found)
            break;
        if (prev)
            prev->next = curr->next;
        else
            free_lists[block->level] = curr->next;
        if (block > buddy)
            block = buddy;
        block->level++;
    }
    block->is_free = true;
    block->next = free_lists[block->level];
    free_lists[block->level] = block;
}

void buddy_free(void* ptr) {
    if (!ptr)
        return;
    struct buddy_block_header *block = (struct buddy_block_header *)((uint8_t*)ptr - sizeof(struct buddy_block_header));
    block->is_free = true;
    block->next = free_lists[block->level];
    free_lists[block->level] = block;
    merge_block(block);
}

void buddy_allocator_destroy() {
    if (buddy_region)
        munmap(buddy_region, buddy_region_size);
    buddy_region = NULL;
    buddy_region_size = 0;
    if (free_lists) {
        free(free_lists);
        free_lists = NULL;
    }
}