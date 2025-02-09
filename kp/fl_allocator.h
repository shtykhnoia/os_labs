#ifndef FL_ALLOCATOR_H
#define FL_ALLOCATOR_H

#include <stddef.h>

void fl_allocator_init(size_t initial_size);
void fl_allocator_destroy();
void* fl_malloc(size_t size);
void fl_free(void* ptr);

#endif