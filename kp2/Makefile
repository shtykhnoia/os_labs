# Компилятор и флаги для компиляции
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

# Список исходных файлов для основного приложения и тестов
MAIN_SRCS = main.c mem_common.c fl_allocator.c buddy_allocator.c buddy_block.c
TEST_SRCS = test_allocators.c mem_common.c fl_allocator.c buddy_allocator.c buddy_block.c

# Преобразуем имена исходных файлов в объектные файлы
MAIN_OBJS = $(MAIN_SRCS:.c=.o)
TEST_OBJS = $(TEST_SRCS:.c=.o)

# Имена результирующих исполняемых файлов
TARGET = allocator
TEST_TARGET = test_allocators

# Цель по умолчанию собирает оба исполняемых файла
all: $(TARGET) $(TEST_TARGET)

$(TARGET): $(MAIN_OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN_OBJS)

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJS)

# Правило компиляции для исходного файла в объектный файл
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(TEST_TARGET) $(MAIN_OBJS) $(TEST_OBJS)

rebuild: clean all