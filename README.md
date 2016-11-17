# IFJ16 interpret

## About
IFJ16 is a four-to-five-members school project, assigned in __Formal languages and compilers__ subject. The goal is create
an interpreter for fictive language IFJ16 whose grammar can be gound in __doc/LL-grammar.txt__. 

The implementation has been split up into several modules:
	* scanner = lexical analysis
	* parser = syntactic & semantic analysis & code generation
	* op-parser = a sub-parser for parsing expressions
	* interpreter = a module handling the execution of compiled program

## How to compile

### How to run the parser
'gcc str.c ial.c stable.c scanner.c parser.c error.c -std=c99 -o p'
'./p FILE'

## Authors
	xbarna02 - Barna Kristian
	xdobia11 - Dobias Roman
	xsulca00 - Sulc Adam	
	xtomas32 - Tomasov Adrian
	xurban66 - Urbanovsky Jozef
	
