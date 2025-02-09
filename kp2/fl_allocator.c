// fl_allocator.c
#include "fl_allocator.h"
#include "mem_common.h"
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

// Объединение вместимости и размера блока
typedef struct { size_t bytes; } block_capacity;
typedef struct { size_t bytes; } block_size;

// Структура заголовка блока, теперь с двусвязным списком
// Содержит гибкий массив для данных, который следует сразу после заголовка
struct block_header {
    struct block_header* next;
    struct block_header* prev;
    block_capacity capacity;   // Вместимость блока только для данных (без заголовка)
    bool is_free;
    uint8_t contents[];        // Данные блока
};

#define REGION_MIN_SIZE (1 << 20) // Минимальный размер региона: 1 МБ
#define BLOCK_MIN_CAPACITY 24     // Минимальная вместимость блока

// Вычисление полного размера блока (заголовок + данные)
static inline block_size size_from_capacity(block_capacity cap) {
    return (block_size){ cap.bytes + offsetof(struct block_header, contents) };
}

// Глобальные переменные для региона и списка свободных блоков
static void* fl_region = NULL;
static size_t fl_region_size = 0;
// Список содержит только свободные блоки
static struct block_header* fl_free_list = NULL;

// Функция для добавления свободного блока в двусвязный список (вставка в начало)
static void add_free_block(struct block_header* block) {
    block->prev = NULL;
    block->next = fl_free_list;
    if (fl_free_list)
        fl_free_list->prev = block;
    fl_free_list = block;
}

void fl_allocator_init(size_t initial_size) {
    size_t req = initial_size > REGION_MIN_SIZE ? initial_size : REGION_MIN_SIZE;
    fl_region_size = round_pages(req);
    fl_region = map_pages(NULL, fl_region_size, 0);
    if (fl_region == MAP_FAILED) {
        perror("mmap fl_region");
        exit(EXIT_FAILURE);
    }
    // Инициализируем один большой свободный блок, покрывающий весь регион.
    // Он сразу же становится единственным элементом free list.
    fl_free_list = (struct block_header*)fl_region;
    fl_free_list->next = NULL;
    fl_free_list->prev = NULL;
    fl_free_list->is_free = true;
    fl_free_list->capacity.bytes = fl_region_size - offsetof(struct block_header, contents);
}

void* fl_malloc(size_t size) {
    if (size == 0)
        return NULL;
    if (size < BLOCK_MIN_CAPACITY)
        size = BLOCK_MIN_CAPACITY;

    struct block_header* curr = fl_free_list;
    // Поиск свободного блока с достаточной вместимостью
    while (curr) {
        if (curr->capacity.bytes >= size) {
            size_t total_size = size + offsetof(struct block_header, contents);
            // Если блок можно разделить, и остаётся место для нового свободного блока, разделяем:
            if (curr->capacity.bytes >= total_size + sizeof(struct block_header) + BLOCK_MIN_CAPACITY) {
                size_t remaining_capacity = curr->capacity.bytes - total_size;
                struct block_header* new_block = (struct block_header*)((uint8_t*)curr->contents + size);
                new_block->is_free = true;
                new_block->capacity.bytes = remaining_capacity;
                // Вставляем новый блок в список свободных блоков
                new_block->next = curr->next;
                new_block->prev = curr->prev;  // Так как curr будет выделен, его ссылки не нужны
                if (curr->next)
                    curr->next->prev = new_block;
                // Если curr был головой списка, обновляем её
                if (fl_free_list == curr)
                    fl_free_list = new_block;
                curr->capacity.bytes = size;
            } else {
                // Если блок точно подходит по размеру, убираем его из free list.
                if (curr->prev)
                    curr->prev->next = curr->next;
                else
                    fl_free_list = curr->next;
                if (curr->next)
                    curr->next->prev = curr->prev;
            }
            curr->next = curr->prev = NULL;
            curr->is_free = false;
            return (void*)curr->contents;
        }
        curr = curr->next;
    }
    // Если подходящий блок не найден, возвращаем NULL
    return NULL;
}

void fl_free(void* ptr) {
    if (!ptr)
        return;
    // Определяем заголовок блока по указателю на данные
    struct block_header* block = (struct block_header*)((uint8_t*)ptr - offsetof(struct block_header, contents));
    block->is_free = true;

    // При попытке объединения удалённого ранее блока в free list, блок уже не должен находиться в списке.
    // Объединяем со следующим блоком, если он свободен
    if (block->next && block->next->is_free) {
        struct block_header* next_block = block->next;
        block->capacity.bytes += offsetof(struct block_header, contents) + next_block->capacity.bytes;
        block->next = next_block->next;
        if (next_block->next)
            next_block->next->prev = block;
    }
    // Объединяем с предыдущим блоком, если он свободен
    if (block->prev && block->prev->is_free) {
        struct block_header* prev_block = block->prev;
        prev_block->capacity.bytes += offsetof(struct block_header, contents) + block->capacity.bytes;
        prev_block->next = block->next;
        if (block->next)
            block->next->prev = prev_block;
        block = prev_block;
    }
    // Вставляем объединённый блок в free list, если его там ещё нет.
    // Здесь важно, чтобы в списке оставались только свободные блоки.
    if (!block->prev && fl_free_list != block) {
        add_free_block(block);
    }
}

void fl_allocator_destroy() {
    if (fl_region)
        munmap(fl_region, fl_region_size);
    fl_region = NULL;
    fl_free_list = NULL;
    fl_region_size = 0;
}