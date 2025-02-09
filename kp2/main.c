#include <stdio.h>
#include "fl_allocator.h"
#include "buddy_allocator.h"

#define INITIAL_SIZE (1 << 20)  // 1 МБ

int main(void) {
    // Тестирование первой стратегии (fl_allocator)
    printf("Тестирование первой стратегии (fl_allocator)...\n");
    fl_allocator_init(INITIAL_SIZE);
    void *p1 = fl_malloc(100);
    void *p2 = fl_malloc(200);
    if (p1 && p2) {
        printf("Выделены блоки: %p и %p\n", p1, p2);
    } else {
        printf("Ошибка выделения памяти в первой стратегии.\n");
    }
    fl_free(p1);
    fl_free(p2);
    fl_allocator_destroy();
    
    // Тестирование второй стратегии (buddy_allocator)
    printf("\nТестирование второй стратегии (buddy_allocator)...\n");
    buddy_allocator_init(INITIAL_SIZE);
    void *q1 = buddy_malloc(100);
    void *q2 = buddy_malloc(200);
    if (q1 && q2) {
        printf("Выделены блоки: %p и %p\n", q1, q2);
    } else {
        printf("Ошибка выделения памяти во второй стратегии.\n");
    }
    buddy_free(q1);
    buddy_free(q2);
    buddy_allocator_destroy();
    
    return 0;
}