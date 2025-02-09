// buddy_block.h
#ifndef BUDDY_BLOCK_H
#define BUDDY_BLOCK_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MIN_BUDDY_SIZE 32  // Минимальный размер блока

// Структура заголовка для buddy‑аллокатора
struct buddy_block_header {
    unsigned int level;  // Уровень блока
    bool is_free;
    struct buddy_block_header *next;
    struct buddy_block_header *prev;
};

// Вставка блока в двусвязный список свободных блоков (сортировка по адресу)
void bb_insert_free_block(struct buddy_block_header **free_list, struct buddy_block_header *block);

// Удаление блока из двусвязного списка свободных блоков
void bb_remove_free_block(struct buddy_block_header **free_list, struct buddy_block_header *block);

// Разделение блока: делит блок уровня level на два блока уровня (level - 1)
void bb_split_block(unsigned int level, struct buddy_block_header **free_lists);

// Вычисление адреса buddy для данного блока на уровне level с учётом региона
struct buddy_block_header* bb_buddy_of(struct buddy_block_header *block, unsigned int level, void *region);

// Объединение свободного блока с его buddy, если тот свободен
void bb_merge_block(struct buddy_block_header **free_lists, unsigned int buddy_max_level, void *region, struct buddy_block_header *block);

#endif // BUDDY_BLOCK_H