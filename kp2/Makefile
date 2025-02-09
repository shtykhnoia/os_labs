# Компилятор и флаги для компиляции
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

# Список исходных файлов проекта
SRCS = main.c mem_common.c fl_allocator.c buddy_allocator.c buddy_block.c
TEST_SRCS = test_allocators.c

# Преобразуем имена исходных файлов в объектные файлы
OBJS = $(SRCS:.c=.o)
TEST_OBJS = $(TEST_SRCS:.c=.o)

# Имя результирующего исполняемого файла
TARGET = allocator
TEST_TARGET = test_allocators

# Правило по умолчанию: сборка исполняемого файла
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(TEST_TARGET): $(OBJS) $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(OBJS) $(TEST_OBJS)

# Правило компиляции исходного файла в объектный файл
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(TEST_TARGET) $(OBJS) $(TEST_OBJS)

rebuild: clean all