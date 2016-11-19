STACK=dynamic_stack
LEX=scanner
PARSER=op-parser
PROGRAM=parser
IAL=ial
STRING=str
ERROR=error
ERROR_OP=error_op
STABLE=stable

DIR=op_prec_praser

all:	
	rm -rf *.o ${PROGRAM}
	
	gcc -std=c11 -Wall -g -Wextra -pedantic utils/${STACK}.c -c
	gcc -std=c11 -Wall -g -Wextra -pedantic ../${ERROR}.c -c
	gcc -std=c11 -Wall -g -Wextra -pedantic ../${STRING}.c -c
	gcc -std=c11 -Wall -g -Wextra -pedantic ../${IAL}.c -c
	#gcc -std=c11 -Wall -g -Wextra -pedantic ../${PROGRAM}.c -c
	gcc -std=c11 -Wall -g -Wextra -pedantic ../${LEX}.c -c	
	gcc -std=c11 -Wall -g -Wextra -pedantic ../${STABLE}.c -c	
	gcc -std=c11 -Wall -g -Wextra -pedantic ${PARSER}.c -c
	gcc -std=c11 -o ${PROGRAM}  ${IAL}.o ${STABLE}.o ${STACK}.o ${ERROR}.o ${STRING}.o ${LEX}.o ${PARSER}.o


clean:
	rm -rf *.o ${PROGRAM}
