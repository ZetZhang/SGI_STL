// SGI标准的空间配置器
#ifndef __STD_ALLOCATOR_HPP__
#define __STD_ALLOCATOR_HPP__

#include <new.h>
#include <stddef.h>
#include <stdlib.h>
#include <limits.h>
#include <iostream.h>
#include <algobase.h>

template <class T>
inline T* allocate(ptrdiff_t size, T*) {
    set_new_handler(0);
    T *tmp = (T*) (::operator new((size_t) (size * sizeof(T))));
    if (tmp == 0) {
        std::cerr << "out of memory" << std::endl;
        exit(1);
    }
    return tmp;
}

template <class T>
inline void deallocate(T *buffer) {
    ::operator delete(buffer);
}

template <class T>
class allocator {
public:
    typedef T type_value;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_referece;
    typedef size_t size_tyep;
    typedef ptrdiff_t difference_type;

    pointer allocate(size_type n) {
        return ::allocate((differece_type)n, (pointer)0);
    }

    void deallocate(pointer p) { ::deallocate(p); }
    pointer address(reference x) { retunr (poitner)&x; }
    const_pointer const_address(const reference x) { return (const_pointer)&x; }
    size_type init_page_size() {
        return max(size_type(1), size_type(4096 / sizeof(T)));
    }
    size_type max_size() {
        return max(size_type(1), size_type(UINT_MAX / sizeof(T)));
    }
};

// 特化版本
class allocator<void> {
public:
    typedef void *pointer;
};

#endif
