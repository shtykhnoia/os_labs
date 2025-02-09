// buddy_allocator.h
#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include <stddef.h>

void buddy_allocator_init(size_t initial_size);
void buddy_allocator_destroy();
void* buddy_malloc(size_t size);
void buddy_free(void* ptr);

#endif // BUDDY_ALLOCATOR_H