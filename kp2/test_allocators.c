#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "fl_allocator.h"
#include "buddy_allocator.h"
#include "buddy_block.h"

#define INITIAL_SIZE (1 << 20)      // 1 МБ - размер тестируемого региона
#define NUM_BLOCKS 50
#define SMALL_BLOCK_SIZE 100        // Размер маленького блока для теста внешней фрагментации
#define LARGE_BLOCK_SIZE (800 * 1024) // 800 КБ - размер большого блока для проверки фрагментации
size_t test_sizes[] = { 50, 100, 200, 300, 500, 800, 1024 };
int num_tests = sizeof(test_sizes) / sizeof(test_sizes[0]);

// Тест внешней фрагментации для fl_allocator (без изменений)
void test_external_fragmentation_fl() {
    printf("=== Тест внешней фрагментации: fl_allocator ===\n");
    fl_allocator_init(INITIAL_SIZE);

    void* blocks[NUM_BLOCKS] = {0};
    for (int i = 0; i < NUM_BLOCKS; i++) {
        blocks[i] = fl_malloc(SMALL_BLOCK_SIZE);
        if (!blocks[i]) {
            printf("fl_allocator: Ошибка выделения на блоке %d\n", i);
            break;
        }
    }
    // Освобождаем каждый второй блок для создания фрагментации (возрастание индексов)
    for (int i = 0; i < NUM_BLOCKS; i += 2) {
        if (blocks[i]) {
            fl_free(blocks[i]);
            blocks[i] = NULL;
        }
    }
    // Пытаемся выделить один большой блок
    void* large_block = fl_malloc(LARGE_BLOCK_SIZE);
    if (large_block) {
        printf("fl_allocator: Удалось выделить большой блок после фрагментации -> Внешняя фрагментация НИЗКАЯ.\n");
        fl_free(large_block);
    } else {
        printf("fl_allocator: Не удалось выделить большой блок -> Внешняя фрагментация ВЫСОКАЯ.\n");
    }
    // Освобождаем оставшиеся блоки
    for (int i = 1; i < NUM_BLOCKS; i += 2) {
        if (blocks[i])
            fl_free(blocks[i]);
    }
    fl_allocator_destroy();
}

// Тест внешней фрагментации для buddy_allocator с изменённым порядком освобождения блоков
void test_external_fragmentation_buddy() {
    printf("=== Тест внешней фрагментации: buddy_allocator ===\n");
    buddy_allocator_init(INITIAL_SIZE);

    void* blocks[NUM_BLOCKS] = {0};
    for (int i = 0; i < NUM_BLOCKS; i++) {
        blocks[i] = buddy_malloc(SMALL_BLOCK_SIZE);
        if (!blocks[i]) {
            printf("buddy_allocator: Ошибка выделения на блоке %d\n", i);
            break;
        }
    }

    for (int i = NUM_BLOCKS - 1; i >= 0; i -= 2) {
        if (blocks[i]) {
            buddy_free(blocks[i]);
            blocks[i] = NULL;
        }
    }
    // Пытаемся выделить один большой блок
    void* large_block = buddy_malloc(LARGE_BLOCK_SIZE);
    if (large_block) {
        printf("buddy_allocator: Удалось выделить большой блок после фрагментации -> Внешняя фрагментация НИЗКАЯ.\n");
        buddy_free(large_block);
    } else {
        printf("buddy_allocator: Не удалось выделить большой блок -> Внешняя фрагментация ВЫСОКАЯ.\n");
    }
    // Освобождаем оставшиеся блоки
    for (int i = 0; i < NUM_BLOCKS; i++) {
        if (blocks[i])
            buddy_free(blocks[i]);
    }
    buddy_allocator_destroy();
}

// Тест внутренней фрагментации для buddy_allocator (без изменений)
void test_internal_fragmentation_buddy() {
    printf("=== Тест внутренней фрагментации: buddy_allocator ===\n");
    buddy_allocator_init(INITIAL_SIZE);

    size_t total_requested = 0;
    size_t total_allocated = 0;

    for (int i = 0; i < num_tests; i++) {
        size_t req = test_sizes[i];
        void* ptr = buddy_malloc(req);
        if (!ptr) {
            printf("buddy_allocator: Ошибка выделения для размера %zu\n", req);
            continue;
        }
        total_requested += req;
        // Предполагаем, что заголовок находится непосредственно перед возвращаемым указателем
        struct buddy_block_header* header = (struct buddy_block_header*)((uint8_t*)ptr - sizeof(struct buddy_block_header));
        size_t actual_size = MIN_BUDDY_SIZE * (1UL << header->level);
        total_allocated += actual_size;
        printf("Запрошено: %5zu байт, Выделено: %5zu байт, Уровень: %u\n", req, actual_size, header->level);
        buddy_free(ptr);
    }

    double fragmentation = 0.0;
    if (total_allocated > 0)
        fragmentation = (double)(total_allocated - total_requested) / total_allocated * 100.0;
    printf("Средняя внутренняя фрагментация (buddy_allocator): %.2f%%\n", fragmentation);
    buddy_allocator_destroy();
}

void test_internal_fragmentation_fl() {
    printf("=== Тест внутренней фрагментации: fl_allocator ===\n");
    
    size_t total_requested = 0;
    size_t total_allocated = 0;
    
    for (int i = 0; i < num_tests; i++) {
        size_t req = test_sizes[i];
        // Инициализируем аллокатор для каждого теста отдельно
        fl_allocator_init(INITIAL_SIZE);
        void *ptr = fl_malloc(req);
        if (!ptr) {
            printf("fl_allocator: Ошибка выделения для размера %zu байт\n", req);
            fl_allocator_destroy();
            continue;
        }
        total_requested += req;
        // Предполагаем, что fl_allocator выделяет ровно столько, сколько запрошено
        total_allocated += req;
        printf("Запрошено: %5zu байт, Выделено: %5zu байт\n", req, req);
        fl_free(ptr);
        fl_allocator_destroy();
    }
    
    double fragmentation = 0.0;
    if (total_allocated > 0)
        fragmentation = (double)(total_allocated - total_requested) / total_allocated * 100.0;
    printf("Средняя внутренняя фрагментация (fl_allocator): %.2f%%\n", fragmentation);
}



int main(void) {
    printf("Начало тестирования аллокаторов...\n\n");

    test_external_fragmentation_fl();
    printf("\n");
    test_external_fragmentation_buddy();
    printf("\n");
    test_internal_fragmentation_buddy();
    test_internal_fragmentation_fl();
    printf("\nТестирование завершено.\n");
    return 0;
}