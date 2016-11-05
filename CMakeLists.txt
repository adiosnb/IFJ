project(IFJ)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -std=c99")

set(SOURCE_FILES instruction_list.c instruction_list.h instructions.h interpret.c interpret.h
        stack.c stack.h interpret_idea.c)

set(INTER_IDEA_REC instruction_list.c instruction_list.h instructions.h interpret.c interpret.h
        stack.c stack.h inter_table.c inter_table.h inter_idea_recursion.c inter_idea_recursion.h)

set(READ_TEST instruction_list.c instruction_list.h instructions.h interpret.c interpret.h
        stack.c stack.h inter_table.c inter_table.h read_test.c)


add_executable(main ${SOURCE_FILES})
add_executable(interpret_idea ${SOURCE_FILES})
add_executable(inter_idea_recursion ${INTER_IDEA_REC})
add_executable(read_test ${READ_TEST})