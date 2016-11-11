project(IFJ)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -std=c99")

set(SOURCE_FILES instruction_list.c instruction_list.h instructions.h interpret.c interpret.h
        stack.c stack.h interpret_idea.c)

set(INTER_IDEA_REC instruction_list.c instruction_list.h instructions.h interpret.c interpret.h
        stack.c stack.h inter_table.c inter_table.h inter_idea_recursion.c inter_idea_recursion.h)

set(READ_TEST instruction_list.c instruction_list.h instructions.h interpret.c interpret.h
        stack.c stack.h inter_table.c inter_table.h read_test.c)
set(str_test str.c str.h str_test.c)

set(my_scanner scanner_alt.c scanner-unit_alt.c str.c )
set(scanner scanner.c scanner-unit.c str.c)

set(stable_test stable-test.c stable.c stack.h)

add_executable(main ${SOURCE_FILES})
add_executable(interpret_idea ${SOURCE_FILES})
add_executable(inter_idea_recursion ${INTER_IDEA_REC})
add_executable(read_test ${READ_TEST})
add_executable(str_test ${str_test})
add_executable(my_scanner ${my_scanner})
add_executable(scanner ${scanner})
add_executable(stable-test ${stable_test})