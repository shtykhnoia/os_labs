// buddy_block.c
#include "buddy_block.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

// Вставка блока в отсортированный по адресу двусвязный список
void bb_insert_free_block(struct buddy_block_header **free_list, struct buddy_block_header *block) {
    block->is_free = true;
    block->next = NULL;
    block->prev = NULL;
    if (*free_list == NULL || (uintptr_t)block < (uintptr_t)*free_list) {
        block->next = *free_list;
        if (*free_list)
            (*free_list)->prev = block;
        *free_list = block;
        return;
    }
    struct buddy_block_header *curr = *free_list;
    while (curr->next && (uintptr_t)curr->next < (uintptr_t)block)
        curr = curr->next;
    block->next = curr->next;
    if (curr->next)
        curr->next->prev = block;
    block->prev = curr;
    curr->next = block;
}

// Удаление блока из двусвязного списка свободных блоков
void bb_remove_free_block(struct buddy_block_header **free_list, struct buddy_block_header *block) {
    if (block->prev)
        block->prev->next = block->next;
    else
        *free_list = block->next;
    if (block->next)
        block->next->prev = block->prev;
    block->next = block->prev = NULL;
}

// Разделение блока: делит блок уровня level на два блока уровня (level - 1)
void bb_split_block(unsigned int level, struct buddy_block_header **free_lists) {
    if (level == 0 || free_lists[level] == NULL)
        return;
    struct buddy_block_header *block = free_lists[level];
    bb_remove_free_block(&free_lists[level], block);
    unsigned int new_level = level - 1;
    size_t block_size = MIN_BUDDY_SIZE * (1UL << level);
    size_t half_size = block_size / 2;
    struct buddy_block_header *first = block;
    first->level = new_level;
    first->is_free = true;
    first->next = first->prev = NULL;
    struct buddy_block_header *second = (struct buddy_block_header *)((uint8_t*)block + half_size);
    second->level = new_level;
    second->is_free = true;
    second->next = second->prev = NULL;
    bb_insert_free_block(&free_lists[new_level], first);
    bb_insert_free_block(&free_lists[new_level], second);
}

// Вычисление адреса buddy для блока на заданном уровне
struct buddy_block_header* bb_buddy_of(struct buddy_block_header *block, unsigned int level, void *region) {
    size_t block_size = MIN_BUDDY_SIZE * (1UL << level);
    uintptr_t offset = (uintptr_t)block - (uintptr_t)region;
    uintptr_t buddy_offset = offset ^ block_size;
    return (struct buddy_block_header *)((uint8_t *)region + buddy_offset);
}

// Объединение свободного блока с его buddy, если последний также свободен
void bb_merge_block(struct buddy_block_header **free_lists, unsigned int buddy_max_level, void *region, struct buddy_block_header *block) {
    while (block->level < buddy_max_level) {
        struct buddy_block_header *buddy = bb_buddy_of(block, block->level, region);
        struct buddy_block_header *curr = free_lists[block->level];
        bool found = false;
        while (curr) {
            if (curr == buddy) {
                found = true;
                break;
            }
            curr = curr->next;
        }
        if (!found)
            break;
        bb_remove_free_block(&free_lists[block->level], buddy);
        if ((uintptr_t)buddy < (uintptr_t)block)
            block = buddy;
        block->level++;
    }
    block->is_free = true;
    bb_insert_free_block(&free_lists[block->level], block);
}