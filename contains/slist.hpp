#ifndef __SLIST_HPP__
#define __SLIST_HPP__

#include "algorithm.hpp"
#include "../allocator/allocator.hpp"
#include "../allocator/constructor.hpp"

namespace ich
{


// 单向链表的节点基本结构
struct __slist_node_base
{
    __slist_node_base *next;
};
// 单向链表的节点结构
template <class T>
struct __slist_node : public __slist_node_base
{
    T data;
};
// 已知某节点，将新节点置其后
inline __slist_node_base* __slist_make_link(__slist_node_base *prev_node, __slist_node_base *new_node) {
    new_node->next = prev_node->next;
    prev_node->next = new_node;
    return new_node;
}
// 链表大小
inline size_t __slist_size(__slist_node_base *node) {
    size_t result = 0;
    for (; node != 0; node = node->next)
        ++result;
    return result;
}
// 单向链表迭代器基本结构
struct __slist_iterator_base
{
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef forward_iterator_tag iterator_category;
    __slist_node_base *node;
    __slist_iterator_base(__slist_node_base *x) : node(x) {}
    void incr() { node = node->next; }
    bool operator==(const __slist_iterator_base &x) const { return node == x.node; }
    bool operator!=(const __slist_iterator_base &x) const { return node != x.node; }
};
// 单向链表迭代器结构
template <class T, class Ref, class Ptr>
struct __slist_iterator : public __slist_iterator_base
{
    typedef __slist_iterator<T, T&, T*>             iterator;
    typedef __slist_iterator<T, const T&, const T*> const_iterator;
    typedef __slist_iterator<T, Ref, Ptr>           self;
    typedef T               value_type;
    typedef Ptr             pointer;
    typedef Ref             reference;
    typedef __slist_node<T> list_node;

    __slist_iterator(): __slist_iterator_base(0) {}
    __slist_iterator(list_node *x) : __slist_iterator_base(x) {}
    __slist_iterator(const iterator &x) : __slist_iterator_base(x.node) {}

    reference operator*() const { return ((list_node*) node)->data; }
    reference operator->() const { return &(operator*()); }
    // 没有实现operator--()因为这是forward iterator
    self& operator++() {
        incr();
        return *this;
    }
    self& operator++(int) {
        self tmp = *this;
        incr();
        return tmp;
    }
};
// slist数据结构
template <class T, class Alloc = alloc>
class slist
{
public:
    typedef T                   value_type;
    typedef value_type*         pointer;
    typedef const value_type*   const_pointer;
    typedef value_type&         reference;
    typedef const value_type&   const_reference;
    typedef size_t              size_type;
    typedef ptrdiff_t           difference_type;

    typedef __slist_iterator<T, T&, T*>             iterator;
    typedef __slist_iterator<T, const T&, const T*> const_iterator;

    slist() { head.next = 0; }
    ~slist() { clear(); }
    iterator begin() { return iterator((list_node*)head.next); }
    iterator end() { return iterator(0); }
    size_type size() const { return  __slist_size(head.next); }
    bool empty() const { return head.next == 0; }
    reference front() { return ((list_node*) head.next)->data; }
    void push_front(const value_type &x) { __slist_make_link(&head, create_node(x)); }
    void pop_front() {
        list_node* node = (list_node*) head.next;
        head.next = node->next;
        destroy_node(node);
    }
    void insert(iterator it, const value_type &x) {
        list_node *node = (list_node*)head.next;
        if (node->data == *it) { push_front(x); return; }
        for (; node->next != 0 && ((list_node*)node->next)->data != *it; node = (list_node*)node->next);
        list_node *new_node = create_node(x);
        new_node->next = node->next;
        node->next = new_node;
    }
    iterator erase(iterator it) {
        list_node *node;
        for (node = (list_node*)head.next; node->next != 0 && ((list_node*)node->next)->data != *it; node = (list_node*)node->next);
        list_node *t = (list_node*)node->next;
        node->next = t->next;
        t->next = 0;
        return iterator((list_node*)t);
    }
    void clear() {
        list_node *node = (list_node*)head.next;
        while (node != 0) {
            list_node *tmp = node;
            node = (list_node*)node->next;
            destroy_node(tmp);
        }
        head.next = (list_node*)0;
    }
    // 两个slist互换
    void swap(slist &L) {
        list_node_base *tmp = head.next;
        head.next = L.head.next;
        L.head.next = tmp;
    }

protected:
private:
    typedef __slist_node<T>         list_node;
    typedef __slist_node_base       list_node_base;
    typedef __slist_iterator_base   iterator_base;
    typedef allocator<list_node>    list_node_allocator;

    list_node_base head;

    static list_node* create_node(const value_type &x) {
        list_node *node = list_node_allocator::allocate();
        construct(&node->data, x);
        node->next = 0;
        // __STL_TRY {} __STL_UNWIND(list_node_allocator::deallocate(node));
        return node;
    }

    static void destroy_node(list_node *node) {
        destroy(&node->data);
        list_node_allocator::deallocate(node);
    }
};


}


#endif
