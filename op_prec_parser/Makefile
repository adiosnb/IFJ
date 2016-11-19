STACK=dynamic_stack
LEX=scanner
PARSER=op-parser
PROGRAM=parser
IAL=ial
STRING=str
ERROR=error

DIR=op_prec_praser

all:
	pushd ..
	
	gcc -std=c11 -Wall -g -Wextra -pedantic op_prec_parser/utils/${STACK}.c -c
	gcc -std=c11 -Wall -g -Wextra -pedantic -o ${DIR}/${ERROR} ${ERROR}.c -c
	gcc -std=c11 -Wall -g -Wextra -pedantic -o ${DIR}/${STRING} ${STRING}.c -c
	gcc -std=c11 -Wall -g -Wextra -pedantic -o ${DIR}/${IAL} ${IAL}.c -c
	gcc -std=c11 -Wall -g -Wextra -pedantic -o ${DIR}/${LEX} ${LEX}.c -c	
	gcc -std=c11 -Wall -g -Wextra -pedantic ${DIR}/${PARSER}.c -c
	gcc -std=c11 -o ${DIR}/${PROGRAM} ${DIR}/${IAL}.o ${DIR}/${STACK}.o ${DIR}/${ERROR}.o ${DIR}/${STRING}.o ${DIR}/${LEX}.o ${DIR}/${PARSER}.o
	
	popd

clean:
	rm *.o  ${DIR}/${PROGRAM}
