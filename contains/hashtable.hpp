#ifndef __HASHTABLE_HPP__
#define __HASHTABLE_HPP__

#include "algorithm.hpp"
#include "pair.hpp"
#include "../allocator/allocator.hpp"
#include "../allocator/constructor.hpp"
#include "hash_fun.hpp"
#include "vector.hpp"


namespace ich
{

static const int __stl_num_primes = 28;
static const unsigned long __stl_prime_list[__stl_num_primes] =
{
    53,         97,         193,        389,        769,
    1543,       3079,       6151,       12289,      24593,
    49157,      98317,      196613,     393241,     786433,
    1572869,    3145739,    6291469,    12582917,   25165843,
    50331653,   100663319,  201326611,  402653189,  805306457,
    1610612741, 3221225473ul, 4294967291ul
};
// 找出28个质数中最接近并大于或等于n的质数
inline unsigned long __stl_next_prime(unsigned long n) {
    const unsigned long *first = __stl_prime_list;
    const unsigned long *last = __stl_prime_list + __stl_num_primes;
    const unsigned long *pos = ich::lower_bound(first, last, n);
    return pos == last ? *(last - 1) : *pos;
}

// hashtable节点
template <class Value>
struct __hashtable_node
{
    __hashtable_node *next;
    Value val;
};

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc = allocator<Value>>
class hashtable;
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_iterator;
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_const_iterator;

// hashtable迭代器
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_iterator
{
    typedef ich::hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>                hashtable;
    typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>          iterator;
    typedef __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>    const_iterator;
    typedef __hashtable_node<Value> node;

    typedef forward_iterator_tag    iterator_category;
    typedef Value                   value_type;
    typedef ptrdiff_t               difference_type;
    typedef size_t                  size_type;
    typedef Value&                  reference;
    typedef Value*                  pointer;

    node *cur;
    hashtable *ht;

    __hashtable_iterator(node *n, hashtable *tab) : cur(n), ht(tab) {}
    __hashtable_iterator() {}
    reference operator*() const { return cur->val; }
    pointer operator->() const { return &(operator*()); }
    iterator& operator++();
    iterator operator++(int);
    bool operator==(const iterator &it) const { return cur == it.cur; }
    bool operator!=(const iterator &it) const { return cur != it.cur; }
};

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_const_iterator
{
    typedef ich::hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>                hashtable;
    typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>          iterator;
    typedef __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>    const_iterator;
    typedef __hashtable_node<Value> node;

    typedef forward_iterator_tag    iterator_category;
    typedef Value                   value_type;
    typedef ptrdiff_t               difference_type;
    typedef size_t                  size_type;
    typedef const Value&                  reference;
    typedef const Value*                  pointer;

    const node *cur;
    const hashtable *ht;

    __hashtable_const_iterator(node *n, hashtable *tab) : cur(n), ht(tab) {}
    __hashtable_const_iterator() {}
    reference operator*() const { return cur->val; }
    pointer operator->() const { return &(operator*()); }
    const_iterator& operator++();
    const_iterator operator++(int);
    bool operator==(const iterator &it) const { return cur == it.cur; }
    bool operator!=(const iterator &it) const { return cur != it.cur; }
};

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
__hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>&
__hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::operator++() {
    const node *old = cur;
    cur = cur->next;
    if (!cur) {
        size_type bucket = ht->bkt_num(old->val);
        while (!cur && ++bucket < ht->buckets.size())   // private within，通过friend class访问
            cur = ht->buckets[bucket];
    }
    return *this;
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
inline __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
__hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::operator++(int) {
    iterator tmp = *this;
    ++*this;
    return tmp;
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
__hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>&
__hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::operator++() {
    const node *old = cur;
    cur = cur->next;
    if (!cur) {
        size_type bucket = ht->bkt_num(old->val);
        while (!cur && ++bucket < ht->buckets.size())
            cur = ht->buckets[bucket];
    }
    return *this;
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
inline __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
__hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::operator++(int) {
    iterator tmp = *this;
    ++*this;
    return tmp;
}

// hashtable数据结构
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
class hashtable
{
public:
    typedef Key key_type;
    typedef HashFcn hasher;
    typedef EqualKey key_equal;
    typedef size_t size_type;
    typedef Value value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef ptrdiff_t difference_type;
    typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> iterator;
    typedef __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> const_iterator;
private:
    // function object
    hasher      hash;
    key_equal   equals;
    ExtractKey  get_key;

    typedef __hashtable_node<Value> node;
    typedef allocator<node>         node_allocator;
    ich::vector<node*, Alloc>       buckets;
    size_type                       num_elements;

    friend struct __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
    friend struct __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
public:
    // 判断元素去哪个bucket
    size_type bkt_num_key(const key_type &key, size_t n) { return ich::hash<key_type>()(key) % n; }
    size_type bkt_num_key(const key_type &key) { return bkt_num_key(key, buckets.size()); }
    size_type bkt_num(const value_type &obj) { return bkt_num_key(get_key(obj)); }
    size_type bkt_num(const value_type &obj, size_t n) { return bkt_num_key(get_key(obj), n); }

    size_type bucket_count() const { return buckets.size(); }
    size_type max_bucket_count() const { return __stl_prime_list[__stl_num_primes - 1]; }
    size_type next_size(size_type n) const { return __stl_next_prime(n); }
    // 构造与内存分配
    node* new_node(const value_type &obj) {
        node *n = node_allocator::allocate();
        n->next = 0;
        // STL_TRY STL_UNWIND(deallocate)
        try {
            construct(&n->val, obj);
        } catch (...) {
            node_allocator::deallocate(n);
        }
        return n;
    }
    void delete_node(node *n) {
        destroy(&n->val);
        node_allocator::deallocate(n);
    }
    void initialize_buckets(size_type n) {
        const size_type n_buckets = next_size(n);
        buckets.reserve(n_buckets);
        buckets.insert(buckets.end(), n_buckets, (node *)0);
        num_elements = 0;
    }
    hashtable(size_type n, const HashFcn &hf, const EqualKey &eql) : hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0) {
        initialize_buckets(n);
    }
    ~hashtable() { clear(); }
    // insert & resize
    void resize(size_type num_elements_hint);
    // 插入元素，不允许重复
    pair<iterator, bool> insert_unique_noresize(const value_type &obj);
    pair<iterator, bool> insert_unique(const value_type &obj) {
        resize(num_elements + 1);
        return insert_unique_noresize(obj);
    }
    // 插入元素，允许重复
    iterator insert_equal_noresize(const value_type &obj);
    iterator insert_equal(const value_type &obj) {
        resize(num_elements + 1);
        return insert_equal_noresize(obj);
    }
    void clear();
    void copy_from(const hashtable &ht);
    iterator find(const key_type &key) {
        const size_type n = bkt_num_key(key);
        node *first;
        for (first = buckets[n]; first && !equals(get_key(first->val), key); first = first->next);
        return iterator(first, this);
    }
    size_type count(const key_type &key) {
        const size_type n = bkt_num_key(key);
        size_type result = 0;
        for (const node *cur = buckets[n]; cur; cur = cur->next)
            if (equals(get_key(cur->val), key))
                ++result;
        return result;
    }
    size_type size() const { return num_elements; }
    //size_type max_size() const { return size_type(-1); }
    bool empty() const { return size() == 0; }
    void swap(hashtable &ht) {
        swap(hash, ht.hash);
        swap(equals, ht.equals);
        swap(get_key, ht.get_key);
        buckets.swap(ht.buckets);
        swap(num_elements, ht.num_elements);
    }
    iterator end() { return iterator(0, this); }
    iterator begin() {
        for (size_type n = 0; n < buckets.size(); ++n)
            if (buckets[n])
                return iterator(buckets[n], this);
        return end();
    }
    const_iterator cend() { return iterator(0, this); }
    const_iterator cbegin() const {
        for (size_type n = 0; n < buckets.size(); ++n)
            if (buckets[n])
                return const_iterator(buckets[n], this);
        return cend();
    }
    size_type elems_in_bucket(size_type bucket) const {
        size_type result = 0;
        for (node* cur = buckets[bucket]; cur; cur = cur->next)
            result += 1;
        return result;
    }
};

// 重建表格
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::resize(size_type num_elements_hint) {
    const size_type old_n = buckets.size();
    if (num_elements_hint > old_n) {
        const size_type n = next_size(num_elements_hint);
        if (n > old_n) {
            ich::vector<node*, Alloc> tmp(n, (node*)0);
            // __STL_TRY
            // 处理旧buckets
            for (size_type bucket = 0; bucket < old_n; ++bucket) {
                node *first = buckets[bucket];
                while (first) {
                    size_type new_bucket = bkt_num(first->val, n);  // 寻找落脚点
                    buckets[bucket] = first->next;
                    first->next = tmp[new_bucket];
                    tmp[new_bucket] = first;
                    first = buckets[bucket];
                }
            }
            buckets.swap(tmp);  // 新旧buckets对调
        }
    }
}

// 不需要重建表格的情况下插入节点，不允许重复
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::iterator, bool>
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::insert_unique_noresize(const value_type &obj) {
    const size_type n = bkt_num(obj);
    node *first = buckets[n];
    // 检查重复
    for (node *cur = first; cur; cur = cur->next)
        if (equals(get_key(cur->val), get_key(obj)))
            return pair<iterator, bool>(iterator(cur, this), false);
    node* tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    ++num_elements;
    return pair<iterator, bool>(iterator(tmp, this), true);
}

// 允许重复
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::iterator
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::insert_equal_noresize(const value_type &obj) {
    const size_type n = bkt_num(obj);
    node *first = buckets[n];
    // 与之前相反，遇到重复值直接插入返回
    for (node *cur = first; cur; cur = cur->next) {
        if (equals(get_key(cur->val), get_key(obj))) {
            node *tmp = new_node(obj);
            tmp->next = cur->next;
            cur->next = tmp;
            ++num_elements;
            return iterator(tmp, this);
        }
    }
    node *tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    ++num_elements;
    return iterator(tmp, this);
}

// clear
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::clear() {
    for (size_type i = 0; i < buckets.size(); ++i)  {
        node *cur = buckets[i];
        while (cur != 0) {
            node *next = cur->next;
            delete_node(cur);
            cur = next;
        }
        buckets[i] = 0;
    }
    num_elements = 0;
}

// copy
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::copy_from(const hashtable &ht) {
    // 先clear buckets，比对空间大小，不够调整，再copy
    // 此时buckets为空，尾端就是开头处
    buckets.reserve(ht.buckets.size());
    buckets.insert(buckets.end(), ht.buckets.size(), (node*)0);
    // __STL_TRY {} __STL_UNWIND(clear())
    try {
        // 遍历ht.buckets，每一个指针都有一个链表
        for (size_type i = 0; i < ht.buckets.size(); ++i) {
            if (const node *cur = ht.buckets[i]) {
                node *copy = new_node(cur->val);
                buckets[i] = copy;
                for (node *next = cur->next; next; cur = next, next = cur->next) {
                    copy->next = new_node(next->val);
                    copy = copy->next;
                }
            }
        }
    } catch (...) {
        this->clear();
    }
}




}



#endif
