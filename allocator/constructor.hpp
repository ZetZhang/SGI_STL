#ifndef __CONSTRUCTOR_HPP__
#define __CONSTRUCTOR_HPP__

#include "__type_traits.hpp"
// 内存基础处理工具
// STL定义有五个作用于未初始化空间的全局函数，分别为：
// construct()、destroy()、uninitialized_copy()、uninitialized_fill()和uninitialized_fill_n()

//#include <stl_alloc.h>        // 负责内存空间的配置与释放
//#include <stl_construct.h>    // 负责对象内容的构造与析构
//#include <stl_uninitialized>  // 三个底层函数
#include "uninitialized.hpp"

namespace ich {

template <class T1, class T2>
inline void construct(T1 *p, const T2 &value) {
    // placement new：从已分配好的内存中构造对象
    new (p) T1(value);  // T1::T1(value)
}

// 如果元素的value type有non-trivial destructor
template <class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) {
    for (; first < last; ++ first)
        destroy(&*first);
}

// 如果元素的value type有trivial destructor
template <class ForwardIterator>
inline void __destroy_aux(ForwardIterator, ForwardIterator, __true_type) {}

// 判断元素的value type是否有trivial destructor
template <class ForwardIterator, class T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*) {
    typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
    __destroy_aux(first, last, trivial_destructor());
}

// 第一版本，接收两个指针
template <class T>
inline void destroy(T *pointer) {
    pointer->~T();
}

// 第二版本，接收两个迭代器
template <class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
    __destroy(first, last, value_type(first));
}

// destroy特化版本
inline void destory(char *, char *) {}
inline void destory(wchar_t *, wchar_t *) {}

}

#endif
