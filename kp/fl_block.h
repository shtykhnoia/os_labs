#ifndef FL_BLOCK_H
#define FL_BLOCK_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* Структура заголовка блока для free-list аллокатора */
struct fl_block_header {
    struct fl_block_header *next;
    size_t capacity;  // Размер полезной области (без учета заголовка)
    bool is_free;
    uint8_t contents[];
};

#define FL_HEADER_SIZE (offsetof(struct fl_block_header, contents))
#define BLOCK_MIN_CAPACITY 24

/* Разделяет блок, если он слишком большой для запроса */
bool fl_split_block(struct fl_block_header *block, size_t query);

/* Пытается объединить (слияние) данный блок со следующим, если они свободны и смежны */
bool fl_try_merge_with_next(struct fl_block_header *block);

#endif