project(IFJ)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic ")

set(SOURCE_FILES  instruction_list.c  instruction_list.h  interpret.c  interpret.h  main.c)

add_executable(main ${SOURCE_FILES})