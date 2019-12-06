#ifndef __DEQUE_HPP__
#define __DEQUE_HPP__

#include "algorithm.hpp"
#include "../allocator/allocator.hpp"
#include "../allocator/constructor.hpp"

namespace ich
{

#define INITIAL_MAP_SIZE 8
// 如果n不为0，使用自定义buffer size，否则根据value_type大小设默认值
inline size_t __deque_buf_size(size_t n, size_t sz) {
    return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
}

template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {
    typedef __deque_iterator<T, T&, T*, BufSiz>             iterator;
    typedef __deque_iterator<T, const T&, const T*, BufSiz> const_iterator;
    typedef __deque_iterator<T, Ref, Ptr, BufSiz>           self;
    static size_t buffer_size() { return __deque_buf_size(BufSiz, sizeof(T)); }
    typedef random_access_iterator_tag  iterator_category;
    typedef T                   value_type;
    typedef Ref                 reference;
    typedef Ptr                 pointer;
    typedef size_t              size_type;
    typedef ptrdiff_t           difference_type;
    typedef pointer*            map_pointer;
    // 保持与容器的连接
    T *cur;
    T *first;
    T *last;
    map_pointer node;

    // 如果行进时达到缓冲区边缘，视前进还是后退来决定是否调用set_node()跳缓冲区
    void set_node(map_pointer new_node) {
        node = new_node;
        first = *new_node;
        last = first + difference_type(buffer_size());
    }
    // 运算符重载
    reference operator*() const { return *cur; }
    pointer operator->() const { return &(operator*()); }
    difference_type operator-(const self &x) const {
        return difference_type(buffer_size()) * (node - x.node - 1) + (cur - first) + (x.last - x.cur);
    }
    self& operator++() {
        ++cur;
        if (cur == last) {
            set_node(node + 1);
            cur = first;
        }
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }
    self& operator--() {
        if (cur == first) {
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }
    // 随机存取,使map中分散的缓冲区看起来连贯
    self& operator+=(difference_type n) {
        difference_type offset = n + (cur - first);
        if (offset >= 0 && offset < difference_type(buffer_size()))
            cur += n;
        else {
            difference_type node_offset = offset > 0 ?
                offset / difference_type(buffer_size()) :
                -difference_type((-offset - 1) / buffer_size()) - 1;
            set_node(node + node_offset);
            cur = first + (offset - node_offset * difference_type(buffer_size()));
        }
        return *this;
    }
    self operator+(difference_type n) const {
        self tmp = *this;
        return tmp += n;
    }
    self& operator-=(difference_type n) { return *this += -n; }
    self operator-(difference_type n) const {
        self tmp = *this;
        return tmp -= n;
    }
    reference operator[](difference_type n) const { return *(*this + n); }
    bool operator==(const self &x) const { return cur == x.cur; }
    bool operator!=(const self &x) const { return !(*this == x); }
    bool operator<(const self &x) const { return (node == x.node) ? (cur < x.cur) : (node < x.node); }
};

template <class T, class Alloc = allocator<T>, size_t BufSiz = 0>
class deque {
public:
    typedef T           value_type;
    typedef T*          pointer;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef __deque_iterator<T, T&, T*, BufSiz> iterator;
    typedef __deque_iterator<const T, const T&, const T*, BufSiz> const_iterator;

    // 除了维护指向mpa的指针，还维护了start和finish，分别指向第一个缓冲区的第一个元素和最后一个缓冲区的最后一个元素
    // 当map提供的节点不足时，就会重新分配一块map
    iterator begin() { return start; }
    iterator end() { return finish; }
    // call __deque_iterator<>::operator[]
    // call __deque_iterator<>::operaotr*
    reference operator[](size_type n) { return start[difference_type(n)]; }
    reference front() { return *start; }
    const_reference cfront() const { return *start; }
    reference back() {
        iterator tmp = finish;
        --tmp;
        return *tmp;    // 不用*(finish - 1)是因为__deque_iterator<>没有为它定义运算符
    }
    const_reference cback() const {
        const_iterator tmp = finish;
        --tmp;
        return *tmp;
    }
    size_type size() const { return finish - start; }   // iterator::operator-
    size_type max_size() const { return size_type(-1); }
    bool empty() const { return finish == start; }

    deque(int n, const value_type &value) : start(), finish(), map(0), map_size(0) { fill_initialize(n, value); }

    void push_back(const value_type &t) {
        if (finish.cur != finish.last - 1) {// 如果map最后有两个元素以上的备用空间
            construct(finish.cur, t);
            ++finish.cur;
        } else {                            // 最后缓冲区只剩一个元素备用空间
            push_back_aux(t);
        }
    }

    void push_front(const value_type &t) {
        if (start.cur != start.first) {
            construct(start.cur - 1, t);
            --start.cur;
        } else {                            // 第一个缓冲区没有任何元素
            push_front_aux(t);
        }
    }

    void pop_back() {
        if (finish.cur != finish.first) {
            --finish.cur;           // 调整指针
            destroy(finish.cur);    // 析构元素
        } else {
            pop_back_aux();                 // 最后缓冲区没有元素
        }
    }

    void pop_front() {
        if (start.cur != start.last - 1) {
            destroy(start.cur);
            ++start.cur;
        } else {                            // 第一个缓冲区只有一个元素
            pop_front_aux();
        }
    }

    void clear();
    // 清除某个元素
    iterator erase(iterator pos);
    // 清除[first, last)区间元素
    iterator erase(iterator first, iterator last);
    iterator insert(iterator position, const value_type &x);
protected:
    // 每次配置一个元素大小
    typedef allocator<value_type>   data_allocator;
    // 每次配置一个指针大小
    typedef allocator<pointer>      map_allocator;
    // poitner of pointer of T
    typedef pointer*    map_pointer;
    map_pointer map;    // map是连续空间，每个元素都是指针，指向缓冲区
    size_type map_size; // map可容纳多少指针
    iterator start;
    iterator finish;

    size_type initial_map_size() { return INITIAL_MAP_SIZE; }
    pointer allocate_node() { return data_allocator::allocate(sizeof(pointer)); }
    void deallocate_node(pointer x) { data_allocator::deallocate(x, 1); }
    // 产生deque结构并设定初值
    void fill_initialize(size_type n, const value_type &value);
    // 产生并安排好deque结构
    void create_map_and_nodes(size_type num_elements);
    // 重新配置一个map
    void reallocate_map(size_type nodes_to_add, bool add_at_front);
    void push_back_aux(const value_type &t);
    void push_front_aux(const value_type &t);
    void pop_back_aux();
    void pop_front_aux();
    // map整治
    void reserve_map_at_bacK(size_type nodes_to_add = 1) {
        // 如果map尾空间不够，则新创建并拷贝到map，删除旧map
        if (nodes_to_add + 1 > map_size - (finish.node - map))
            reallocate_map(nodes_to_add, false);
    }
    void reserve_map_at_front(size_type nodes_to_add = 1) {
        // 同上，条件是前端节点不够
        if (nodes_to_add > start.node - map)
            reallocate_map(nodes_to_add, true);
    }
    iterator insert_aux(iterator pos, const value_type &x);
};

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::fill_initialize(size_type n, const value_type &value) {
    create_map_and_nodes(n);
    map_pointer cur;
    // no__STL_TRY {} catch (...) {}
    // 为每个节点缓冲区设置初值
    for (cur = start.node; cur < finish.node; ++cur)
        uninitialized_fill(*cur, *cur + iterator::buffer_size(), value);
    // 最后节点的设定不同
    uninitialized_fill(finish.first, finish.cur, value);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements) {
    // 如果需要的节点数是被整除的结果，会多分配一个节点
    size_type num_nodes = num_elements / iterator::buffer_size() + 1;
    // map管理最少8个节点，最多所需节点+2，前后各一个作扩充用
    map_size = ich::max(initial_map_size(), num_nodes + 2);
    map = map_allocator::allocate(map_size);
    // 让nstart和nfinish指向map全部节点的最中央区段，使得头尾两端扩展能量一样大
    map_pointer nstart = map + (map_size - num_nodes) / 2;
    map_pointer nfinish = nstart + num_nodes - 1;
    // 为map内的每个节点分配缓冲区，综合就是deque的可用空间
    map_pointer cur;
    for (cur = nstart; cur <= nfinish; ++cur)
        *cur = allocate_node();
    // 设置正确节点
    start.set_node(nstart);
    finish.set_node(nfinish);
    start.cur = start.first;
    // 多分配一个字节
    finish.cur = finish.first + num_elements % iterator::buffer_size();
}

// 重新配置一个map
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reallocate_map(size_type nodes_to_add, bool add_at_front) {
    size_type old_num_nodes = finish.node - start.node + 1;
    size_type new_num_nodes = old_num_nodes + nodes_to_add; // perhaps the num is 1

    map_pointer new_nstart;
    if (map_size > 2 * new_num_nodes) { // 位置足够，重新调整位置
        new_nstart = map + (map_size - new_num_nodes) / 2 +
            (add_at_front ? nodes_to_add : 0);
        if (new_nstart < start.node)
            //ich::copy(start.node, finish.node + 1, new_nstart);
            //ich::uninitialized_copy(start.node, finish.node + 1, new_nstart);
            ich::icopy(start.node, finish.node + 1, new_nstart);
        else
            ich::copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
    } else {                            // 配置一块空间给新map使用
        // mapsize还+max()+2? 双倍扩容
        size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;
        map_pointer new_map = map_allocator::allocate(new_map_size);
        new_nstart = new_map + (new_map_size - new_num_nodes) / 2 +
            (add_at_front ? nodes_to_add : 0);  // 寻找合适的中间位置
        // 将原map内容拷贝到new_nstart
        ich::icopy(start.node, finish.node + 1, new_nstart);
        // 释放原map
        map_allocator::deallocate(map, map_size);
        // 设定新map起始地址和大小
        map = new_map;
        map_size = new_map_size;
    }
    // 重新设置start和finish迭代器
    start.set_node(new_nstart);
    finish.set_node(new_nstart + old_num_nodes - 1);
}

// 重新调整中间结构，再分配back缓冲区
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_back_aux(const value_type &t) {
    value_type t_copy = t;
    reserve_map_at_bacK();  // 整治map空间：back
    *(finish.node + 1) = allocate_node();
    // __STL_TRY {} catch(...) {}
    try {
        construct(finish.cur, t_copy);
        finish.set_node(finish.node + 1);
        finish.cur = finish.first;
    } catch (...) {
        deallocate_node(*(finish.node + 1));
    }
}

// 重新调整中间结构，再分配front缓冲区
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_front_aux(const value_type &t) {
    value_type t_copy = t;
    reserve_map_at_front(); // 整治map空间：front
    *(start.node - 1) = allocate_node();
    // _STL_TRY {} catch (...) {}
    try {
        start.set_node(start.node - 1);
        start.cur = start.last - 1;
        construct(start.cur, t_copy);
    } catch (...) {
        start.set_node(start.node + 1);
        start.cur = start.first;
        deallocate_node(*(start.node - 1));
        throw;
    }
}

// 最后缓冲区没有空间，调整finish指向上一个缓冲区的最后一个元素
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_back_aux() {
    deallocate_node(finish.first);      // 释放缓冲区
    finish.set_node(finish.node - 1);   // 上一个缓冲区最后一个元素
    finish.cur = finish.last - 1;
    destroy(finish.cur);
}

// 第一个缓冲区只有一个元素，调整finish指向下一个缓冲区第一个元素
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_front_aux() {
    destroy(start.cur);                 // 析构第一个缓冲区第一个元素
    deallocate_node(start.first);       // 释放缓冲区
    start.set_node(start.node + 1);
    start.cur = start.first;
}

// clear()清除整个deque，deque最初有一个缓冲区，因此需要保留一个缓冲区
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::clear() {
    // 针对除头尾外的每个缓冲区
    for (map_pointer node = start.node + 1; node < finish.node; ++node) {
        destroy(*node, *node + iterator::buffer_size());
        data_allocator::deallocate(*node, iterator::buffer_size());
    }
    if (start.node != finish.node) {// 存有头尾两个缓冲区
        destroy(start.cur, start.last);
        destroy(finish.first, finish.cur);
        data_allocator::deallocate(finish.first, iterator::buffer_size());// 保留头缓冲区
    } else {                        // 只有一个缓冲区
        destroy(start.cur, finish.cur);
    }
    finish = start; // 调整
}

template <class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator
deque<T, Alloc, BufSize>::erase(iterator pos) {
    iterator next = pos;
    ++next;
    difference_type index = pos - start;
    // 看index所处位置靠前靠后，确保移动次数最少
    if (index < (size() >> 1)) {// pos前元素少
        ich::copy_backward(start, pos, next);   // start-pos |--> next
        pop_front();
    } else {                    // pos后元素少
        ich::icopy(next, finish, pos);          // pos <--| next-finish
        pop_back();
    }
    return start + index;
}

template <class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator
deque<T, Alloc, BufSize>::erase(iterator first, iterator last) {
    if (first == start && last == finish) {
        clear();
        return finish;
    } else {
        difference_type n = last - first;               // 区间长度
        difference_type elems_before = first - start;   // 区间前元素个数
        // 覆盖清除区间，向前向后取决于前方和后方那边元素最少
        if (elems_before < (size() - n) / 2) {
            ich::copy_backward(start, first, last); // start-first |--> last
            iterator new_start = start + n;
            destroy(start, new_start);
            for (map_pointer cur = start.node; cur < new_start.node; ++cur)
                data_allocator::deallocate(*cur, iterator::buffer_size());
            start = new_start;  // 调整
        } else {
            ich::icopy(last, finish, first);        // first <--| last-finish
            iterator new_finish = finish - n;
            destroy(new_finish, finish);
            for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
                data_allocator::deallocate(*cur, iterator::buffer_size());
            finish = new_finish;
        }
        return start + elems_before;
    }
}

template <class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator
deque<T, Alloc, BufSize>::insert(iterator position, const value_type &x) {
    if (position.cur == start.cur) {        // 最前端
        push_front(x);
        return start;
    } else if (position.cur == finish.cur) {// 最尾端
        push_back(x);
        iterator tmp = finish;
        --tmp;
        return tmp;
    } else {                                // insert_aux()处理
        return insert_aux(position, x);
    }
}

template <class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator
deque<T, Alloc, BufSize>::insert_aux(iterator pos, const value_type &x) {
    difference_type index = pos - start;
    value_type x_copy = x;
    // 移动腾出插入点
    if (index < size() / 2) {
        push_front(front());    // 前插一个元素，值与第一个元素相同
        auto front1 = start;
        ++front1;
        iterator front2 = front1;
        ++front2;
        pos = start + index;
        iterator pos1 = pos;
        ++pos1; // 前闭后开
        ich::icopy(front2, pos1, front1);       // front1 <--| front2-pos1
    } else {
        push_back(back());      // 后插一个元素
        iterator back1 = finish;
        --back1;
        auto back2 = back;
        --back2;
        pos = start + index;
        ich::copy_backward(pos, back2, back1);  // pos-back2 |--> back1
    }
    *pos = x_copy;
    return pos;
}


}

#endif
