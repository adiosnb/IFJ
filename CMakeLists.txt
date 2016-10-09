project(IFJ)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -std=c99")

set(SOURCE_FILES instruction_list.c instruction_list.h instructions.h interpret.c interpret.h scanner.c scanner.h
        stack.c stack.h stable.h interpret_idea.c)

add_executable(main ${SOURCE_FILES})
add_executable(interpret_idea ${SOURCE_FILES})