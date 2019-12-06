#ifndef __ITERATOR_HPP__
#define __ITERATOR_HPP__

#include <cstddef>

namespace ich {
//只是一个tag，在迭代器操作函数的参数中提供，用于让函数根据不同的迭代器选择最优的操作
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

template<class Category, class T, class Distance = ptrdiff_t, class Pointer = T *, class Reference = T &>
struct iterator {
    typedef Category    iterator_category;
    typedef T           value_type;
    typedef Distance    difference_type;
    typedef Pointer     pointer;
    typedef Reference   reference;
};

// traits
template<class Iterator>
struct iterator_traits {
    typedef typename Iterator::iterator_category    iterator_category;
    typedef typename Iterator::value_type           value_type;
    typedef typename Iterator::difference_type      difference_type;
    typedef typename Iterator::pointer              pointer;
    typedef typename Iterator::reference            reference;
};

// 对原生指针的偏特化版本
template<class T>
struct iterator_traits<T*> {
    typedef random_access_iterator_tag  iterator_category;
    typedef T                           value_type;
    typedef ptrdiff_t                   difference_type;
    typedef T*                          pointer;
    typedef T&                          reference;
};
template<class T>
struct iterator_traits<const T*> {
    typedef random_access_iterator_tag  iterator_category;
    typedef T                           value_type;
    typedef ptrdiff_t                   difference_type;
    typedef const T*                    pointer;
    typedef const T&                    reference;
};

// iterator_category
template<class Iterator>
inline typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator&) {
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return category();
}

// value_type
template<class Iterator>
inline typename iterator_traits<Iterator>::value_type* value_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

// difference_type
template<class Iterator>
inline typename iterator_traits<Iterator>::difference_type* difference_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

// distance()函数

// input_iterator_tag distance
template<class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type __distance(InputIterator first, InputIterator last, input_iterator_tag) {
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

// random_access_iterator_tag distance
template<class RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type __distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
    return last - first;
}

//命名要求，以能接受的最低阶迭代器名作为模板名称
template<class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type distance(InputIterator first, InputIterator last, input_iterator_tag) {
    typedef typename iterator_traits<InputIterator>::iterator_category category;
    return __distance(first, last, category());
}

// distance function
template <class InputIterator, class Distance>
inline void __distance(InputIterator first, InputIterator last, Distance &n, input_iterator_tag) {
    while (first != last) {
        ++first;
        ++n;
    }
}

template <class RandomAccessIterator, class Distance>
inline void __distance(RandomAccessIterator first, RandomAccessIterator last, Distance &n, random_access_iterator_tag tag) {
    n += last - first;
}

template <class InputIterator, class Distance>
inline void distance(InputIterator first, InputIterator last, Distance &n) {
    __distance(first, last, n, iterator_category(first));
}

template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type distance(InputIterator first, InputIterator last) {
    typedef typename iterator_traits<InputIterator>::iterator_category category;
    return __distance(first, last, category());
}

// advance()函数

// input_iterator_tag advance
template<class InputIterator, class Distance>
inline void __advance(InputIterator& i, Distance n, input_iterator_tag) {
    while (n--) {
        ++i;
    }
}

// bidirectional_iterator_tag advance
template<class BidirectionalIterator, class Distance>
inline void __advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag) {
    if (n >= 0) {
        while (n--) {
            ++i;
        }
    } else {
        while (n++) {
            --i;
        }
    }
}

// random_access_iterator_tag advance
template<class RandomAccessIterator, class Distance>
inline void __advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag) {
    i += n;
}

template<class InputIterator, class Distance>
inline void advance(InputIterator& i, Distance n) {
    typedef typename iterator_traits<InputIterator>::iterator_category category;
    __advance(i, n, category());
}

}

#endif // ITERATOR_H

