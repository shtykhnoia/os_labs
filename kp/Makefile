CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

OBJS = main.o fl_allocator.o buddy_allocator.o fl_block.o

all: allocators

allocators: $(OBJS)
	$(CC) $(CFLAGS) -o allocators $(OBJS)

main.o: main.c fl_allocator.h buddy_allocator.h
	$(CC) $(CFLAGS) -c main.c

fl_allocator.o: fl_allocator.c fl_allocator.h mem_common.h fl_block.h
	$(CC) $(CFLAGS) -c fl_allocator.c

buddy_allocator.o: buddy_allocator.c buddy_allocator.h mem_common.h
	$(CC) $(CFLAGS) -c buddy_allocator.c

fl_block.o: fl_block.c fl_block.h
	$(CC) $(CFLAGS) -c fl_block.c

clean:
	rm -f *.o allocators