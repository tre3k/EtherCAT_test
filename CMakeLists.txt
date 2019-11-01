cmake_minimum_required(VERSION 2.8.12)

project(ether-test)

set(INCLUDES include)
set(LIBS lib)

find_library(PTHREAD_LIB pthread)
set(OSLIBS ${PTHREAD_LIB})

include_directories(${INCLUDES})
link_directories(${LIBS})

add_executable(ether-test main.c)
target_link_libraries(ether-test soem ${OSLIBS})


