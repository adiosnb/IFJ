STACK=dynamic_stack
LEX=scanner
PARSER=op-parser
PROGRAM=parser
IAL=ial
STRING=str
ERROR=error

all:
	gcc -std=c11 -Wall -g -Wextra -pedantic utils/${STACK}.c -c
	gcc -std=c11 -Wall -g -Wextra -pedantic error/${ERROR}.c -c
	gcc -std=c11 -Wall -g -Wextra -pedantic scanner/${STRING}.c -c
	gcc -std=c11 -Wall -g -Wextra -pedantic scanner/${IAL}.c -c
	gcc -std=c11 -Wall -g -Wextra -pedantic scanner/${LEX}.c -c	
	gcc -std=c11 -Wall -g -Wextra -pedantic op-parser/${PARSER}.c -c
	gcc -std=c11 -o ${PROGRAM} ${IAL}.o ${STACK}.o ${ERROR}.o ${STRING}.o ${LEX}.o ${PARSER}.o

clean:
	rm *.o  ${PROGRAM}
