project(IFJ)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -std=c99")

set(SOURCE_FILES instruction_list.c instruction_list.h instructions.h interpret.c interpret.h
        stack.c stack.h)

set(str_test str.c str.h str_test.c)

set(my_scanner scanner_alt.c scanner-unit_alt.c str.c )
set(scanner scanner.c scanner-unit.c str.c)
set(interpret-expr-test interpret.c interpret-expr-test.c str.c instruction_list.c stack.c stable.c)
set(stable_test stable-test.c stable.c stack.h)
set(interpret-str-test interpret-str-test.c str.c interpret.c stack.c stable.c instruction_list.c instructions.h)

set(str-list-test str.c str-list-test.c)


add_executable(str_test ${str_test})
add_executable(my_scanner ${my_scanner})
add_executable(scanner ${scanner})
add_executable(interpret-expr-test ${interpret-expr-test})
add_executable(stable-test ${stable_test})

add_executable(interpret-str-test ${interpret-str-test})

add_executable(str-list-test ${str-list-test})