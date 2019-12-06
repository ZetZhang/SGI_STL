#ifndef __ALGORITHM_HPP__
#define __ALGORITHM_HPP__

#include "../allocator/iterator.hpp"
#ifdef DEBUG
#include <iostream>
#endif

namespace ich {

template <typename T>
T max(const T &left, const T &right) {
    return left > right ? left : right;
}

template <class BidirIt1, class BidirIt2>
BidirIt2 copy_backward(BidirIt1 first, BidirIt1 last, BidirIt2 d_last) {
    while (first != last)
        *(--d_last) = *(--last);
    return d_last;
}


template <class InputIt, class OutputIt>
OutputIt icopy(InputIt first, InputIt last, OutputIt d_first) {
    while (first != last)
        *d_first++ = *first++;
    return d_first;
}


template<class InputIt, class T>
InputIt find(InputIt first, InputIt last, const T& value)
{
    for (; first != last; ++first) {
        if (*first == value) {
            return first;
        }
    }
    return last;
}

// less
template <class T>
struct less
{
    bool operator()(const T &lhs, const T &rhs) const { return lhs < rhs; }
};

// grater
template <class T>
struct greater
{
    bool operator()(const T &lhs, const T &rhs) const { return lhs > rhs; }
};

// identity
template <class T>
struct identity /*: public unary_function<T, T> */
{
    const T& operator()(const T &x) const { return x; }
    const T&& operator()(const T &&x) const { return x; }
};

// equal_to??
template <class T>
struct equal_to
{
    bool operator()(const T &lhs, const T &rhs) const { return lhs == rhs; }
};

// swap
template <typename T1, typename T2>
void swap(T1 &left, T2 &right) {
    T1 &tmp = left;
    left = right;
    right = tmp;
}

// binary search
// 版本一
template <class ForwardIterator, class T, class Distance>
ForwardIterator __lower_bound (ForwardIterator first, ForwardIterator last, const T& value, Distance*, forward_iterator_tag)
{
    Distance len;
    __initialize(len, Distance(0));
    distance(first, last, len);
    Distance half;
    ForwardIterator middle;

    while (len > 0)
    {
        half = len / 2;
        middle = first;
        advance(middle, half);
        if (*middle < value)
        {
            first = middle;
            ++first;
            len = len - half - 1;
        }
        else
            len = half;
    }
    return first;
}

template <class RandomAccessIterator, class T, class Distance>
RandomAccessIterator __lower_bound (RandomAccessIterator first, RandomAccessIterator last, const T& value, Distance*, random_access_iterator_tag)
{
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle;

    while (len > 0)
    {
        half = len / 2;
        middle = first + half;
        if (*middle < value)
        {
            first = middle + 1;
            len = len - half - 1;
        }
        else
            len = half;
    }
    return first;
}

// 版本二
template <class ForwardIterator, class T, class Compare, class Distance>
ForwardIterator __lower_bound (ForwardIterator first, ForwardIterator last, const T& value, Compare comp, Distance*, forward_iterator_tag)
{
    Distance len;
    __initialize(len, Distance(0));
    distance(first, last, len);
    Distance half;
    ForwardIterator middle;

    while (len > 0)
    {
        half = len / 2;
        middle = first;
        advance(middle, half);
        if (comp(*middle, value))
        {
            first = middle;
            ++first;
            len = len - half - 1;
        }
        else
            len = half;
    }
    return first;
}

template <class RandomAccessIterator, class T, class Compare, class Distance>
RandomAccessIterator __lower_bound (RandomAccessIterator first, RandomAccessIterator last, const T& value, Compare comp, Distance*, random_access_iterator_tag)
{
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle;

    while (len > 0)
    {
        half = len / 2;
        middle = first + half;
        if (comp(*middle, value))
        {
            first = middle + 1;
            len = len - half - 1;
        }
        else
            len = half;
    }
    return first;
}

// 版本一
template <class ForwardIterator, class T>
inline ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T &value) {
    typedef typename iterator_traits<ForwardIterator>::difference_type* distance_type;
    return __lower_bound(first, last, value, distance_type(first), iterator_category(first));
}
// 版本二
template <class ForwardIterator, class T, class Compare>
inline ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T &value, Compare comp) {
    typedef typename iterator_traits<ForwardIterator>::difference_type* distance_type;
    return __lower_bound(first, last, value, comp, distance_type(first), iterator_category(first));
}

// push_heap()

// max-heap做法
template <class RandomAccessIterator, class Distance, class T>
void __push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value) {
    Distance parent = (holeIndex - 1) / 2;  // 得到父节点位置
    // 不断上溯，父子交换值，直到上溯的父节点值没有新值大
    while (holeIndex > topIndex && *(first + parent) < value){
        *(first + holeIndex) = *(first + parent);
        holeIndex = parent;
        parent = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = value;   // 赋值，完成操作
}

template <class RandomAccessIterator, class Distance, class T, class Compare>
void __push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value, Compare comp) {
    Distance parent = (holeIndex - 1) / 2;  // 得到父节点位置
    // 不断上溯，父子交换值，直到上溯的父节点值没有新值大
    while (holeIndex > topIndex && comp(*(first + parent), value)){
        *(first + holeIndex) = *(first + parent);
        holeIndex = parent;
        parent = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = value;   // 赋值，完成操作
}

template <class RandomAccessIterator, class Distance, class T>
inline void __push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance *, T *) {
    // Distance((last - first) -
    // 1)指最后的节点的位置(新值在底部)，T(*(last - 1))则取得该位置的值（新值），Distance(0)是heap顶部
    __push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)));
}

template <class RandomAccessIterator, class Distance, class T, class Compare>
inline void __push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance *, T *, Compare comp) {
    // Distance((last - first) -
    // 1)指最后的节点的位置(新值在底部)，T(*(last - 1))则取得该位置的值（新值），Distance(0)是heap顶部
    __push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)), comp);
}

template <class RandomAccessIterator>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
    typedef typename iterator_traits<RandomAccessIterator>::value_type* value_type;
    typedef typename iterator_traits<RandomAccessIterator>::difference_type* distance_type;
    __push_heap_aux(first, last, distance_type(first), value_type(first));
}

template <class RandomAccessIterator, class Compare>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    typedef typename iterator_traits<RandomAccessIterator>::value_type* value_type;
    typedef typename iterator_traits<RandomAccessIterator>::difference_type* distance_type;
    __push_heap_aux(first, last, distance_type(first), value_type(first), comp);
}


// pop_heap()

// 调整位置
template <class RandomAccessIterator, class Distance, class T>
void __adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value) {
    Distance topIndex = holeIndex;
    Distance secondChild = 2 * holeIndex + 2;   // holeIndex的右孩子位置
    // 不断更新holeIndex下溯，每次最大孩子节点的值赋给holeIndex
    while (secondChild < len) {
        if (*(first + secondChild) < *(first + (secondChild - 1)))
            secondChild--;
        *(first + holeIndex) = *(first + secondChild);
        holeIndex = secondChild;
        secondChild = 2 * (secondChild + 1);    // 先找右孩子
    }
    // 如果右孩子位置不在尾部，则说明只有左孩子(len这里指位置在last+1)
    if (secondChild == len) {
        *(first + holeIndex) = *(first + (secondChild - 1));
        holeIndex = secondChild - 1;
    }
    // 将一开始的尾值重新插入
    __push_heap(first, holeIndex, topIndex, value); // topIndex和value未改变
    // 尾部还有未移走的极大值
#ifdef DEBUG
    std::cout << "DEBUG >> ";
    for (RandomAccessIterator item = first; item != first + len; item++)
        std::cout << *item << ' ';
    std::cout << std::endl;
#endif
}

template <class RandomAccessIterator, class Distance, class T, class Compare>
void __adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value, Compare comp) {
    Distance topIndex = holeIndex;
    Distance secondChild = 2 * holeIndex + 2;
    while (secondChild < len) {
        if (comp(*(first + secondChild), *(first + (secondChild - 1))))
            secondChild--;
        *(first + holeIndex) = *(first + secondChild);
        holeIndex = secondChild;
        secondChild = 2 * (secondChild + 1);
    }
    if (secondChild == len) {
        *(first + holeIndex) = *(first + (secondChild - 1));
        holeIndex = secondChild - 1;
    }
    __push_heap(first, holeIndex, topIndex, value, comp);
}

template <class RandomAccessIterator, class T, class Distance>
inline void __pop_heap(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator result, T value, Distance*) {
    // 设定尾值为根节点值，稍后pop_back()去除
    *result = *first;
    // value为原尾值
    __adjust_heap(first, Distance(0), Distance(last - first), value);
}

template <class RandomAccessIterator, class T, class Distance, class Compare>
inline void __pop_heap(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator result, T value, Distance*, Compare comp) {
    // 设定尾值为根节点值，稍后pop_back()去除
    *result = *first;
    // value为原尾值
    __adjust_heap(first, Distance(0), Distance(last - first), value, comp);
}

template <class RandomAccessIterator, class T>
inline void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*) {
    typedef typename iterator_traits<RandomAccessIterator>::difference_type* distance_type;
    __pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first));
}

template <class RandomAccessIterator, class T, class Compare>
inline void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*, Compare comp) {
    typedef typename iterator_traits<RandomAccessIterator>::difference_type* distance_type;
    __pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first), comp);
}

template <class RandomAccessIterator>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
    typedef typename iterator_traits<RandomAccessIterator>::value_type* value_type;
    __pop_heap_aux(first, last, value_type(first));
}

template <class RandomAccessIterator, class Compare>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    typedef typename iterator_traits<RandomAccessIterator>::value_type* value_type;
    __pop_heap_aux(first, last, value_type(first), comp);
}


// sort_heap()
template <class RandomAccessIterator>
inline void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
    // 每次pop_heap()，尾部得到该操作的极大值，需要前移，得到排序结果
    while (last - first > 1)
        pop_heap(first, last--);
}

// make_heap()
template <class RandomAccessIterator, class T, class Distance>
void __make_heap(RandomAccessIterator first, RandomAccessIterator last, T *, Distance *) {
    if (last - first < 2) return;   // 长度为0或1不用排序
    Distance len = last - first;
    Distance holeIndex = (len - 2) / 2;
    for (;;) {
        __adjust_heap(first, holeIndex, len, T(*(first + holeIndex)));
        if (holeIndex == 0) return;
        holeIndex--;
    }
}

template <class RandomAccessIterator, class T, class Distance, class Compare>
void __make_heap(RandomAccessIterator first, RandomAccessIterator last, T *, Distance *, Compare comp) {
    if (last - first < 2) return;   // 长度为0或1不用排序
    Distance len = last - first;
    Distance holeIndex = (len - 2) / 2;
    for (;;) {
        __adjust_heap(first, holeIndex, len, T(*(first + holeIndex)), comp);
        if (holeIndex == 0) return;
        holeIndex--;
    }
}

template <class RandomAccessIterator>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
    typedef typename iterator_traits<RandomAccessIterator>::difference_type* distance_type;
    typedef typename iterator_traits<RandomAccessIterator>::value_type* value_type;
    __make_heap(first, last, value_type(first), distance_type(first));
}

// extra
template <class RandomAccessIterator, class Compare>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    typedef typename iterator_traits<RandomAccessIterator>::difference_type* distance_type;
    typedef typename iterator_traits<RandomAccessIterator>::value_type* value_type;
    __make_heap(first, last, value_type(first), distance_type(first), comp);
}


}

#endif
