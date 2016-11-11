STACK=dynamic_stack
LEX=scanner_alt
PARSER=op-parser
PROGRAM=parser
STRING=str
ERROR=error

all:
	gcc -std=c11 -Wall -g -Wextra -pedantic utils/${STACK}.c -c
	gcc -std=c11 -Wall -g -Wextra -pedantic error/${ERROR}.c -c
	gcc -std=c11 -Wall -g -Wextra -pedantic scanner/${STRING}.c -c
	gcc -std=c11 -Wall -g -Wextra -pedantic scanner/${LEX}.c -c	
	gcc -std=c11 -Wall -g -Wextra -pedantic ${PARSER}.c -c
	gcc -std=c11 -o ${PROGRAM} ${STACK}.o ${ERROR}.o ${STRING}.o ${LEX}.o ${PARSER}.o

clean:
	rm *.o  ${PROGRAM}
