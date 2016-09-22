project(IFJ)

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic ")

set(SOURCE_FILES main.cpp)

add_executable(main ${SOURCE_FILES})