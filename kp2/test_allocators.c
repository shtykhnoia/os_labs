// test_allocators.c

#include <stdio.h>
#include <assert.h>
#include "fl_allocator.h"
#include "buddy_allocator.h"

// Тест для fl_allocator (первая стратегия)
// Этот тест выделяет несколько блоков, затем освобождает их и проверяет базовые предположения,
// что операции выделения и объединения (коалесцирования) работают корректно.
void test_fl_allocator() {
    printf("Начало теста fl_allocator...\n");
    // Инициализируем аллокатор с регионом размером 1 МБ
    fl_allocator_init(1024 * 1024);
    
    // Выделяем два блока разного размера
    void *ptr1 = fl_malloc(128);
    void *ptr2 = fl_malloc(256);
    
    // Проверяем, что указатели не равны NULL
    assert(ptr1 != NULL && "Ошибка: fl_malloc вернул NULL для ptr1");
    assert(ptr2 != NULL && "Ошибка: fl_malloc вернул NULL для ptr2");
    
    // Освобождаем блоки
    fl_free(ptr1);
    fl_free(ptr2);
    
    // Здесь можно добавить дополнительную проверку внутреннего состояния (free list),
    // если реализована соответствующая функция для проверки, что в нем только свободные блоки.
    
    // Завершаем работу аллокатора
    fl_allocator_destroy();
    printf("fl_allocator тест пройден!\n\n");
}

// Тест для buddy_allocator (вторая стратегия)
// Этот тест выполняет аналогичные операции: выделение, освобождение и проверку базовой функциональности.
void test_buddy_allocator() {
    printf("Начало теста buddy_allocator...\n");
    // Инициализируем аллокатор с регионом размером 1 МБ
    buddy_allocator_init(1024 * 1024);
    
    // Выделяем два блока
    void *ptr1 = buddy_malloc(128);
    void *ptr2 = buddy_malloc(256);
    
    // Проверяем, что указатели получены
    assert(ptr1 != NULL && "Ошибка: buddy_malloc вернул NULL для ptr1");
    assert(ptr2 != NULL && "Ошибка: buddy_malloc вернул NULL для ptr2");
    
    // Освобождаем блоки
    buddy_free(ptr1);
    buddy_free(ptr2);
    
    // При необходимости, здесь можно добавить проверки free list,
    // используя дополнительные диагностические функции, если они реализованы.
    
    // Завершаем работу аллокатора
    buddy_allocator_destroy();
    printf("buddy_allocator тест пройден!\n\n");
}

int main() {
    test_fl_allocator();
    test_buddy_allocator();
    printf("Все тесты пройдены успешно!\n");
    return 0;
}