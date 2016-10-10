project(IFJ)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic ")

set(SOURCE_FILES instruction_list.c instruction_list.h instructions.h interpret.c interpret.h main.c scanner.c scanner.h
        stack.c stack.h stable.h)

add_executable(main ${SOURCE_FILES})