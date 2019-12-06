#ifndef __UNINITIALIZED_HPP__
#define __UNINITIALIZED_HPP__

#include <cstring>
#include <algorithm>    // fill_n
#include "iterator.hpp"
#include "__type_traits.hpp"
// 这三个函数分别对应高层函数copy()、fill()和fill_n()

namespace ich {


// uninitialized_copy --> construct(&*(result + (i - first)), *i)
//
// function template参数推导，如果不是POD类型，则转入此函数
template <class InputIterator, class ForwardIterator>
inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type) {
    ForwardIterator cur = result;
    for (; first != last; ++first, ++cur)
        construct(&*cur, *first);   // 一个一个元素构造，非POD不能批量生成
    return cur;
}

// 拷贝数组，若元素是trivial copy constructors，则借助memcpy()完成工作
template <class T>
void copy(T *source, T *destination, int n, __true_type) {
    memcpy(destination, source, n);
}

// 元素拥有non-trivial copy constructors
template <class T>
void copy(T *source, T *destination, int n, __false_type) {
    for (; n-->0; ++source, ++destination)
        construct(source, *destination);
}

// copy_n???
template <class T>
inline void copy(T *source, T *destination, int n) {
    copy(source, destination, n, typename __type_traits<T>::has_trivial_copy_constructor());
}

// function template参数推导，如果是POD类型，转入此函数
template <class InputIterator, class ForwardIterator>
inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type) {
    return std::copy(first, last, result);   // 调用STL算法的copy()
}

template <class InputIterator, class ForwardIterator, class T>
inline ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T*) {
    typedef typename __type_traits<T>::is_POD_type is_POD;
    return __uninitialized_copy_aux(first, last, result, is_POD());
}

template <class InputIterator, class ForwardIterator>
ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
    return __uninitialized_copy(first, last, result, value_type(result));
}

// const char*特化版本
template <>
inline char* uninitialized_copy(const char *first, const char *last, char *result) {
    memmove(result, first, last - first);
    return result + (last - first);
}
template <>
inline wchar_t* uninitialized_copy(const wchar_t *first, const wchar_t *last, wchar_t *result) {
    memmove(result, first, sizeof(wchar_t) * (last - first));
    return result + (last - first);
}



// uninitialized_fill

template <class ForwardIterator, class T>
inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &x, __true_type) {
    std::fill(first, last, x);
}

template <class ForwardIterator, class T>
inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &x, __false_type) {
    for (ForwardIterator cur = first; cur != last; ++cur)
        construct(&*cur, x);
}

template <class ForwardIterator, class T, class T1>
inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &x, T1 *) {
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    __uninitialized_fill_aux(first, last, x, is_POD());
}

template <class ForwardIterator, class T>
void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &x) {
    __uninitialized_fill(first, last, x, value_type(first));
}



// uninitialized_fill_n

// POD:Plain Old Data，传统C struct，指trivial ctor/dtor/copy/assignment函数
// 对待传统函数可以使用最有效率的初始化做法，而非POD则需要使用安全做法
// function template参数推导，如果是POD类型，则转入此函数
template <class ForwardIterator, class Size, class T>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, __true_type) {
    return std::fill_n(first, n, x); // 转交高阶函数执行
}
// function template参数推导，如果不是POD类型，则转入此函数
template <class ForwardIterator, class Size, class T>
ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, __false_type) {
    ForwardIterator cur = first;
    for (; n > 0; --n, ++cur)
        construct(&*cur, x);
    return cur;
}

template <class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T &x, T1 *) {
    // __type_traits<>技法
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

template <class ForwardIterator, class Size, class T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T &x) {
    return __uninitialized_fill_n(first, n, x, value_type(first));
}

}

#endif
