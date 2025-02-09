#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "fl_allocator.h"
#include "buddy_allocator.h"

#define NUM_OPS 1000

/* Функция для измерения времени работы аллокатора:
   многократно выделяет и освобождает блоки указанного размера */
double measure_alloc_speed(void* (*alloc_func)(size_t), void (*free_func)(void*), size_t size) {
    void **ptrs = malloc(NUM_OPS * sizeof(void*));
    clock_t start = clock();
    for (int i = 0; i < NUM_OPS; i++) {
        ptrs[i] = alloc_func(size);
        if (!ptrs[i]) {
            fprintf(stderr, "Ошибка выделения памяти\n");
            exit(EXIT_FAILURE);
        }
        memset(ptrs[i], 0, size);
    }
    clock_t mid = clock();
    for (int i = 0; i < NUM_OPS; i++) {
        free_func(ptrs[i]);
    }
    clock_t end = clock();
    free(ptrs);
    double alloc_time = ((double)(mid - start)) / CLOCKS_PER_SEC;
    double free_time = ((double)(end - mid)) / CLOCKS_PER_SEC;
    printf("Время выделения: %.6f сек, время освобождения: %.6f сек\n", alloc_time, free_time);
    return alloc_time + free_time;
}

int main() {
    size_t alloc_size = 64;              // Запрос размером 64 байта
    size_t region_size = 1024 * 1024;      // Инициализация региона 1 МБ

    printf("=== Тест free-list аллокатора ===\n");
    fl_allocator_init(region_size);
    double time_fl = measure_alloc_speed(fl_malloc, fl_free, alloc_size);
    printf("Общее время работы free-list аллокатора: %.6f сек\n", time_fl);
    fl_allocator_destroy();

    printf("\n=== Тест buddy аллокатора ===\n");
    buddy_allocator_init(region_size);
    double time_buddy = measure_alloc_speed(buddy_malloc, buddy_free, alloc_size);
    printf("Общее время работы buddy аллокатора: %.6f сек\n", time_buddy);
    buddy_allocator_destroy();

    return 0;
}