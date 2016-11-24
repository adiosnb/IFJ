STACK=dynamic_stack
LEX=scanner
PARSER=op-parser
PROGRAM=parser
IAL=ial
STRING=str
ERROR=error
ERROR_OP=error_op
BUILD=build

DIR=op_prec_praser

all:	
	mkdir -p build
	rm -rf *.o ${PROGRAM}
	
	gcc -std=c11 -Wall -g -Wextra -pedantic -o ${BUILD}/${STACK}.o utils/${STACK}.c -c
	gcc -std=c11 -Wall -g -Wextra -pedantic -o ${BUILD}/${ERROR}.o ../${ERROR}.c -c
	gcc -std=c11 -Wall -g -Wextra -pedantic -o ${BUILD}/${STRING}.o ../${STRING}.c -c
	gcc -std=c11 -Wall -g -Wextra -pedantic -o ${BUILD}/${IAL}.o ../${IAL}.c -c
	#gcc -std=c11 -Wall -g -Wextra -pedantic ../${PROGRAM}.c -c
	gcc -std=c11 -Wall -g -Wextra -pedantic -o ${BUILD}/${LEX}.o ../${LEX}.c -c	
	gcc -std=c11 -Wall -g -Wextra -pedantic -o ${BUILD}/${PARSER}.o ${PARSER}.c -c
	gcc -std=c11 -o ${PROGRAM}  ${BUILD}/${IAL}.o ${BUILD}/${STACK}.o ${BUILD}/${ERROR}.o ${BUILD}/${STRING}.o ${BUILD}/${LEX}.o ${BUILD}/${PARSER}.o


clean:
	rm -rf *.o ${PROGRAM}
