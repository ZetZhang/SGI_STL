cmake_minimum_required(VERSION 2.0)
project(sgi_stl)
set(CMKAE_BUILD_TYPE "Release")
add_compile_options(-std=c++11)
set(CMAKE_CXX_FLAGS "-g -O3 -Wall")
#include_directories(contains)
#include_directories(allocator)
file(GLOB SOURCES "contains/test.cpp")
#set(SOURCES contains/test.cpp contains/vector.hpp allocator/allocator.hpp
    #allocator/uninitialized.hpp allocator/alloc.hpp allocator/constructor.hpp
    #allocator/iterator.hpp contains/pair.hpp allocator/__type_traits.hpp
    #allocator/iterator.hpp contains/list.hpp contains/deque.hpp
    #contains/queue.hpp contains/stack.hpp contains/priority_queue.hpp
    #contains/slist.hpp contains/set.hpp contains/multiset.hpp
    #contains/hashtable.hpp contains/hash_fun.hpp contains/hashset.hpp)
add_executable(test ${SOURCES})
