cmake_minimum_required(VERSION 3.2)
project(SPSCQueue CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS "true")
set(CMAKE_CXX_STANDARD 11)

if(MSVC)
    add_compile_options(/permissive- /W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_library(SPSCQueue INTERFACE)
target_include_directories(SPSCQueue INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

find_package(Threads REQUIRED)

if (NOT MSVC)
    add_executable(SPSCQueueBenchmark src/SPSCQueueBenchmark.cpp)
    target_link_libraries(SPSCQueueBenchmark SPSCQueue Threads::Threads)
endif()

add_executable(SPSCQueueExample src/SPSCQueueExample.cpp)
target_link_libraries(SPSCQueueExample SPSCQueue Threads::Threads)

add_executable(SPSCQueueTest src/SPSCQueueTest.cpp)
target_link_libraries(SPSCQueueTest SPSCQueue Threads::Threads)

enable_testing()
add_test(SPSCQueueTest SPSCQueueTest)
