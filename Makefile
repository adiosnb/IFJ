all: parser parser-test
bin= parser parser-test
CFLAGS_TEST=-Wall -pedantic -std=c99 -DDEBUG=0 -DTEST -DNOINTERPRET
CFLAGS= -std=c99 -Wall -pedantic
CC=gcc

src=str.c ial.c scanner.c parser.c error.c instruction_list.c interpret.c stack.c op_prec_parser/op-parser.c op_prec_parser/utils/dynamic_stack.c
obj=$(src:.c=.o)

parser-test: $(obj)
	$(CC)  $(src) $(CFLAGS_TEST) -o $@

parser : $(obj)
	$(CC) $(obj) $(CFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o $(bin)
