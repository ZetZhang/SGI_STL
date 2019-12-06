#ifndef __LIST_HPP__
#define __LIST_HPP__

#include "algorithm.hpp"
#include "../allocator/allocator.hpp"
#include "../allocator/constructor.hpp"

namespace ich
{

// 双向链表结构，它也是个环状双向链表
template <class T>
struct __list_node {
    typedef void *void_pointer;
    void_pointer prev;  // __list_node<T>*
    void_pointer next;
    T data;
};

// list迭代器
//template <class T, class Ref, class Ptr>
template <class T>
struct __list_iterator {
    //typedef __list_iterator<T, T&, T*>      iterator;
    //typedef __list_iterator<T, Ref, Ptr>    self;
    // ??
    typedef __list_iterator<T> iterator;
    typedef __list_iterator<T> self;

    // 双向链表必须具备前后移动的能力
    typedef bidirectional_iterator_tag  iterator_category;
    typedef T               value_type;
    //typedef Ptr             pointer;
    //typedef Ref             reference;
    typedef T*              pointer;
    typedef T&              reference;
    typedef __list_node<T>* link_type;
    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;

    link_type node;
    // constructor
    __list_iterator(link_type x) : node(x) {}
    __list_iterator() {}
    __list_iterator(const iterator &x) : node(x.node) {}
    // operator
    bool operator==(const self &x) const { return node == x.node; }
    bool operator!=(const self &x) const { return node != x.node; }
    reference operator*() const { return (*node).data; }
    pointer operator->() const { return &(operator*()); }
    // incr和desc
    self& operator++() {
        node = (link_type)((*node).next);
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }
    self& operator--() {
        node = (link_type)((*node).prev);
        return *this;
    }
    self& operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }
};

// list数据结构
template <class T, class Alloc = allocator<T>>
class list {
public:
    typedef T                        value_type;
    typedef size_t                   size_type;
    typedef T&                       reference;
    typedef __list_node<T>*          link_type;
    typedef __list_iterator<T>       iterator;
    typedef __list_iterator<const T> const_iterator;

    // 产生空链接
    list() { empty_initialize(); }
    iterator insert(iterator position, const T &x) {
        link_type tmp = create_node(x);
        tmp->next = position.node;
        tmp->prev = position.node->prev;
        link_type(position.node->prev)->next = tmp;
        position.node->prev = tmp;
        return tmp;
    }
    iterator erase(iterator position) {
        link_type next_node = position.node->next;
        link_type prev_node = position.node->prev;
        prev_node->next = next_node;
        next_node->prev = prev_node;
        destroy_node(position.node);
        return iterator(next_node);
    }
    void push_front(const T &x) { insert(begin(), x); }
    void push_back(const T &x) { insert(end(), x); }
    void pop_front() { erase(begin()); }
    void pop_back() { iterator tmp = end(); erase(--tmp); }
    iterator begin() { return (link_type)((*node).next); }
    iterator end() { return node; }
    bool empty() const { return node->next == node; }
    size_type size() {
        // raw
        //size_type result = 0;
        //distance(begin(), end(), result);
        //return result;
        //return distance(begin(), end());
        size_type result = 0;
        iterator first =  this->begin();
        iterator last = this->end();
        while (first != last) {
            ++first;
            ++result;
        }
        return result;
    }
    //void what() {
        //--end();
        //std::cout << "what-->--node --> " << node << std::endl;
        //--end();
        //std::cout << "what-->--node --> " << node << std::endl;
        //--end();
        //std::cout << "what-->--node --> " << node << std::endl;
    //}
    reference front() { return *begin(); }
    reference back() { return *(--end()); }
    // 将链表x结合到position之前
    void splice(iterator position, list &x) {
        if (!x.empty())
            transfer(position, x.begin(), x.end());
    }
    // 将i节点放在position之前
    void splice(iterator position, list &, iterator i) {
        iterator j = i;
        ++j;
        if (position == i || position == j) return;
        transfer(position, i, j);
    }
    // 清除整个链表
    void clear() {
        link_type cur = (link_type) node->next;
        while (cur != node) {
            link_type tmp = cur;
            cur = (link_type) cur->next;
            destroy_node(tmp);
        }
        node->next = node;
        node->prev = node;
    }
    // 数值为value的所有元素删除
    void remove(const T &value) {
        iterator first = begin();
        iterator last = end();
        while (first != last) {
            iterator next = first;
            ++next;
            if (*first == value) erase(first);
            first = next;
        }
    }
    // 删除连续的相同元素
    void unique() {
        iterator first = begin();
        iterator last = end();
        if (first == last) return;
        iterator next = first;
        while (++next != last) {
            if (*first == *next)
                erase(next);
            else
                first = next;
            next = first;
        }
    }

protected:
    //typedef __list_node<T> list_node;
    typedef Alloc                   list_node_allocator;
    typedef __list_node<T>          list_node;
    link_type node; // 只需要一个指针，就能实现环状双向链表
    // allcoate
    link_type get_node() { return (link_type)list_node_allocator::allocate(); }
    // deallocate
    void put_node(link_type p) { list_node_allocator::deallocate((T*)p); }
    // allocate and construct with value
    link_type create_node(const T &x) {
        link_type p = get_node();
        construct(&p->data, x);
        return p;
    }
    // destroy and deallocate
    void destroy_node(link_type p) {
        destroy(&p->data);
        put_node(p);
    }
    // 产生空节点，此时begin()在node的next，end()在node的prev
    // 一开始node头尾都指向自己，不设置元素值
    void empty_initialize() {
        node = get_node();
        node->next = node;
        node->prev = node;
    }
    // 将[first, last)内的所有元素移动到position之前
    void transfer(iterator position, iterator first, iterator last) {
        if (position != last) {
            link_type(last.node->prev)->next = position.node;
            link_type(first.node->prev)->next = last.node;
            link_type(position.node->prev)->next = first.node;
            link_type tmp = link_type(position.node->prev);
            position.node->prev = last.node->prev;
            last.node->prev = first.node->prev;
            first.node->prev = tmp;
        }
    }
};


}

#endif
