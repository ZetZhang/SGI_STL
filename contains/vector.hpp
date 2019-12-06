#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__


#include "algorithm.hpp"
#include "../allocator/allocator.hpp"
#include "../allocator/constructor.hpp"

namespace ich {

template <class T, class Alloc = allocator<T>>
class vector {
public:
    typedef T           value_type;
    typedef value_type* pointer;
    typedef value_type* iterator;
    typedef value_type& reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef const value_type* const_iterator;
    typedef const value_type& const_reference;

    iterator begin() { return start; }
    const_iterator cbegin() const { return start; }
    iterator end() { return finish; }
    const_iterator cend() const { return finish; }
    size_type size() const { return size_type(cend() - cbegin()); }
    size_type capacity() const { return size_type(end_of_storage - cbegin()); }
    bool empty() const { return cbegin() == cend(); }
    reference operator[] (size_type n) { return *(begin() + n); }
    const_reference operator[] (size_type n) const { return *(cbegin() + n); }

    vector() : start(0), finish(0), end_of_storage(0) {}
    vector(size_type n, const T &value) { fill_initialize(n, value); }
    vector(int n, const T &value) { fill_initialize(n, value); }
    vector(long n, const T &value) { fill_initialize(n, value); }
    vector(iterator first, iterator last) { fill_initialize(first, last); }
    explicit vector(size_type n) { fill_initialize(n, T()); }
    ~vector() {
        destroy(start, finish);
        deallocate();   // vector member function
    }
    reference front() { return *begin(); }
    reference back() { return *(end() - 1); }
    void insert(iterator position, const T &x);
    void insert(iterator position, size_type n, const T &x);
    void push_back(const T &x) {
        if (finish != end_of_storage) {
            construct(finish, x);
            ++finish;
        } else
            insert_aux(end(), x);
    }
    void pop_back() {
        --finish;
        // finish->~T()
        destroy(finish);
    }
    // 清除[first, last)中的元素
    // 如删除1到4之间即[3, 6)
    // |0|0|0|1|2|3|4|5|6|7|0|0| <--删除前
    // |0|0|0|5|6|7|0|0|0|0|0|0| <--删除前
    iterator erase(iterator first, iterator last) {
        iterator i = copy(last, finish, first);
        destroy(i, finish);
        finish = finish - (last - first);
        return first;
    }
    // 清除某个位置的元素
    iterator erase(iterator position) {
        if (position + 1 != end())
            copy(position + 1, finish, position);
        --finish;
        destroy(finish);
        return position;
    }
    void resize(size_type new_size, const T &x) {
        if (new_size < size())
            erase(begin() + new_size, end());
        else
            insert(end(), new_size - size(), x);
    }
    void resize(size_type new_size) { resize(new_size, T()); }
    void clear() { erase(begin(), end()); }
    void reserve(size_type n);
    void swap(vector<T, Alloc> &x) {
        ich::swap(start, x.start);
        ich::swap(finish, x.finish);
        ich::swap(end_of_storage, x.end_of_storage);
    }
protected:
    //typedef simple_alloc<value_type, Alloc> data_allocator;
    typedef Alloc data_allocator;
    iterator start;
    iterator finish;
    iterator end_of_storage;

    void insert_aux(iterator position, const T &x);
    void deallocate() { if (start) data_allocator::deallocate(start, end_of_storage - start); }
    void fill_initialize(size_type n, const T &value) {
        start = allocate_and_fill(n, value);
        finish = start + n;
        end_of_storage = finish;
    }
    // ignore
    void fill_initialize(iterator first, iterator last) {
        start = allocate_and_fill(first, last);
        finish = start + (last - first);
        end_of_storage = finish;
    }
    // 配置空间并填满内容
    iterator allocate_and_fill(size_type n, const T &x) {
        iterator result = iterator(data_allocator::allocate(n));
        uninitialized_fill_n(result, n, x); // 全局函数
        return result;
    }
    iterator allocate_and_fill(iterator first, iterator last) {
        iterator result = data_allocator::allocate(last - first);
        iterator it, re;
        for (it = first, re = result; it != last; ++it, ++re)
            uninitialized_fill_n(re, 1, *it);
        return result;
    }
    iterator allocate_and_copy(size_type n, iterator first, iterator last) {
        iterator result = iterator(data_allocator::allocate(n));
        uninitialized_copy(first, last, result);
        return result;
    }
};

template <class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const T &x) {
    if (finish != end_of_storage) { // 有备用空间??有备用空间又怎么进来这里？
        construct(finish, *(finish - 1));   // 构造一个元素为什么以最后一个值为初值？
        ++finish;
        T x_copy = x;
        copy_backward(position, finish - 2, finish - 1);
        *position = x_copy;
    } else {    // 无备用空间
        const size_type old_size = size();
        const size_type len = old_size != 0 ? 2 * old_size : 1;
        iterator new_start = data_allocator::allocate(len);
        iterator new_finish = new_start;
        try {
            new_finish = uninitialized_copy(start, position, new_start);
            construct(new_finish, x);
            ++new_finish;
            new_finish = uninitialized_copy(position, finish, new_finish);
        } catch (...) {
            destroy(new_start, new_finish);
            data_allocator::deallocate(new_start, len);
            throw;
        }
        // 析构并释放原来的vector
        destroy(begin(), end());
        deallocate();
        // 指向新的vector
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + len;
    }
}

// 插入单个元素
template <class T, class Alloc>
void vector<T, Alloc>::insert(iterator position, const T &x) {
    if (position >= 0 && position <= end())
        vector<T, Alloc>::insert_aux(position, x);
}

// 从position开始插入n个元素，元素初值为x
template <class T, class Alloc>
void vector<T, Alloc>::insert(iterator position, size_type n, const T &x) {
    if (n != 0) {
        if (size_type(end_of_storage - finish) >= n) {  // 当备用空间足够新增的元素
            T x_copy = x;
            const size_type elems_after = finish - position;
            iterator old_finish = finish;
            if (elems_after > n) {  // 插入点后的元素个数小于需要插入的数量
                uninitialized_copy(finish - n, finish, finish);
                finish += n;
                copy_backward(position, old_finish - n, old_finish);
                uninitialized_fill(position, position + n, x_copy);   // 填充新值
            } else {                // 元素个数大于等于需要插入的数量
                uninitialized_fill_n(finish, n - elems_after, x_copy);
                finish += n - elems_after;
                uninitialized_copy(position, old_finish, finish);
                finish += elems_after;
                uninitialized_fill(position, old_finish, x_copy);
            }
        } else {    // 备用空间不够，需要分配内存
            const size_type old_size = size();
            const size_type len = old_size + max(old_size, n);
            iterator new_start = iterator(data_allocator::allocate(len));
            //iterator new_finish = new_start;
            // 首先将旧vector插入点前的元素copy到新内存
            iterator new_finish = uninitialized_copy(start, position, new_start);
            // 将新增元素（初值n）填入内存
            new_finish = uninitialized_fill_n(new_finish, n, x);
            // 将插入点之后的元素复制到新内存
            new_finish = uninitialized_copy(position, finish, new_finish);
#ifdef __STL_USE_EXCEPTIONS
            catch (...) {
                // 异常发生？？
                destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                throw;
            }
#endif
            // 清除释放旧vector
            destroy(start, finish);
            deallocate();
            // 更新水位
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + len;
        }
    }
}

template <class T, class Alloc>
void vector<T, Alloc>::reserve(size_type n) {
    if (capacity() < n) {
        const size_type old_size = size();
        iterator tmp = allocate_and_copy(n, start, finish);
        destroy(start, finish);
        deallocate();
        start = tmp;
        finish = tmp + old_size;
        end_of_storage = start + n;
    }
}

}

#endif
