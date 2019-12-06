#ifndef __ALLOCATOR_HPP__
#define __ALLOCATOR_HPP__

//#include "constructor.hpp"
#include "alloc.hpp"

namespace ich
{

template<class T>
class allocator
{
 public:
    typedef T               value_type;
    typedef T*              pointer;
    typedef const T*        const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;

    static T* allocate() {
        return static_cast<T*>(alloc::allocate(sizeof(T)));
    }

    static T* allocate(size_t n) {
        return static_cast<T*>(alloc::allocate(sizeof(T) * n));
    }

    static void deallocate(T *ptr) {
        alloc::deallocate(static_cast<T*>(ptr), sizeof(T));
    }

    static void deallocate(T *ptr, size_t n) {
        alloc::deallocate(static_cast<T*>(ptr), sizeof(T) * n);
    }
}; // end of namespace JJ

}

#endif
