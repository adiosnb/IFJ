all: run
bin= run
CFLAGS= -std=c99 -Wall -pedantic
CC=gcc

src=str.c ial.c scanner.c parser.c error.c instruction_list.c interpret.c stack.c op-parser.c dynamic_stack.c
obj=$(src:.c=.o)


run : $(obj)
	$(CC) $(obj) $(CFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o $(bin)
