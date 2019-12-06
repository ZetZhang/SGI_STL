#ifndef __JJ_ALLOC_HPP__
#define __JJ_ALLOC_HPP__

#include <new>      // for placement new / std::set_new_handler()
#include <cstddef>  // for std::ptrdiff_t, std::size_t
#include <cstdlib>  // for std::exit()
#include <climits>  // for UINT_MAX
#include <iostream> // for std::cerr

namespace JJ
{

template<class T>
inline void _deallocate(T *buffer)
{
    ::operator delete(buffer);
}

template<class T1, class T2>
inline void _construct(T1 *p, const T2 &value)
{
    new (p)T1(value);                   // placement new：详见条款52
}

template<class T>
inline void _destroy(T *ptr)
{
    ptr->~T();
}
void whatthefuck()
{
    std::cerr << "what the fuck" << std::endl;
    std::exit(1);
}

template<class T>
inline T* _allocate(std::ptrdiff_t size, T*)
{
    // 安装new-handler函数，当分配失败时调用
    std::set_new_handler(&whatthefuck);  // new-handler行为：详见Effective C++条款49
    T* tmp = (T*)(::operator new ((std::size_t)(size * sizeof(T))));
    if (tmp == 0)
    {
        std::cerr << "out of memory" << std::endl;
        std::exit(1);
    }
    return tmp;
}

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

    // 一个嵌套（nested）class template，只有一个typedef
    template<class U>
    struct rebind
    {
        typedef allocator<U> other;
    };

    // 配置空间，足以存储n个T对象
    pointer allocate(size_type n, const void *hint = 0)
    {
        return _allocate((difference_type)n, (pointer)0);
    }

    // 归还空间
    void deallocate(pointer p, size_type n)
    {
        _deallocate(p);
    }

    void construct(pointer p, const T &value)
    {
        _construct(p, value);
    }

    void destroy(pointer p)
    {
        _destroy(p);
    }

    // 返回某个对象地址
    pointer address(reference x)
    {
        return (pointer)&x;
    }

    // 返回某个const对象的地址
    const_pointer const_address(const_reference x)
    {
        return (const_pointer)&x;
    }

    //size_type init_page_size() {
        //return max(size_type(1), size_type(4096 / sizeof(T)));
    //}

    // 返回可配置的最大容量
    size_type max_size() const
    {
        return size_type(UINT_MAX / sizeof(T));
    }
};

} // end of namespace JJ

#endif
