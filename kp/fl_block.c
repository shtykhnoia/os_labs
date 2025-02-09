#include "fl_block.h"
#include <stddef.h>

bool fl_split_block(struct fl_block_header *block, size_t query) {
    /* Можно разделить, если блок свободный и оставшийся размер достаточно велик */
    if (block->is_free &&
        (query + FL_HEADER_SIZE + BLOCK_MIN_CAPACITY <= block->capacity)) {
        /* Новый блок расположен сразу после выделенной части */
        uint8_t *new_addr = ((uint8_t*)block) + FL_HEADER_SIZE + query;
        struct fl_block_header *new_block = (struct fl_block_header*) new_addr;
        new_block->next = block->next;
        new_block->capacity = block->capacity - query - FL_HEADER_SIZE;
        new_block->is_free = true;
        block->next = new_block;
        block->capacity = query;
        return true;
    }
    return false;
}

bool fl_try_merge_with_next(struct fl_block_header *block) {
    if (block->next && block->is_free && block->next->is_free) {
        /* Определяем конец текущего блока */
        uint8_t *end = ((uint8_t*)block) + FL_HEADER_SIZE + block->capacity;
        if (end == (uint8_t*)block->next) {
            struct fl_block_header *next = block->next;
            block->capacity += FL_HEADER_SIZE + next->capacity;
            block->next = next->next;
            return true;
        }
    }
    return false;
}