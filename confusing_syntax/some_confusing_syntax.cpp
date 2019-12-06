// 一些STL组态
#include <iostream>
#include <cstddef>
#include <vector>
#include <algorithm>
#define iiiiiiiiii std::cout << "----------分隔线----------\n";

class alloc {};
// vector

template <class T, class Alloc = alloc>
class vector {
public:
    typedef T value_type;
    typedef value_type *iterator;

    template <class I>
    void insert(iterator position, I first, I last) {
        std::cout << "insert()" << std::endl;
    }

    void swap(vector<T, Alloc> &)
    {
        std::cout << "swap()" << std::endl;
    }
    static int _data;
};

template <class T, class Alloc>
inline void swap(vector<T, Alloc> &x, vector<T, Alloc> &y) {
    x.swap(y);
}

template<> int vector<int>::_data = 1;
template<> int vector<char>::_data = 2;

// non-type template
inline size_t __deque_buf_size(size_t n, size_t sz)
{
    return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
}

template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {
    typedef __deque_iterator<T, T&, T*, BufSiz> iterator;
    typedef __deque_iterator<T, const T&, const T*, BufSiz> const_iterator;
    static size_t buffer_size() { return __deque_buf_size(BufSiz, sizeof(T)); }
};

// stack && deque
template <class T, class Alloc = alloc, size_t BufSiz = 0>
class deque {
public:
    deque() { std::cout << "deque" << std::endl; }
    // Iterator
    typedef __deque_iterator<T, T&, T*, BufSiz> iterator;
};

// bound friend templates:需要以下前置声明，否则将报错
template <class T, class Sequence>
class stack;

template <class T, class Sequence>
bool operator==(const stack<T, Sequence> &x, const stack<T, Sequence> &y);

template <class T, class Sequence>
bool operator<(const stack<T, Sequence> &x, const stack<T, Sequence> &y);

template <class T, class Sequence = deque<T>>
class stack {
public:
    stack() { std::cout << "stack" << std::endl; }
private:
    // ok
    friend bool operator== <T> (const stack<T> &, const stack<T> &);
    friend bool operator< <T> (const stack<T> &, const stack<T> &);
    // ok
    friend bool operator== <T> (const stack &, const stack &);
    friend bool operator< <T> (const stack &, const stack &);
    // ok
    friend bool operator== <> (const stack &, const stack &);
    friend bool operator< <> (const stack &, const stack &);
    // error
    //friend bool operator== (const stack &, const stack &);
    //friend bool operator< (const stack &, const stack &);
private:
    Sequence c;
};
template <class T, class Sequence>
bool operator==(const stack<T, Sequence> &x, const stack<T, Sequence> &y)
{
    std::cout << "operator==" << '\t';
    return true;
}

template <class T, class Sequence>
bool operator<(const stack<T, Sequence> &x, const stack<T, Sequence> &y)
{
    std::cout << "operator<" << '\t';
    return true;
}

// g++和gcc都不行
// class template explicit specialization:转为不指定template<>
//#define __STL_TEMPLATE_NULL // 为空，不需要template<>，只适用于gcc

//template <class Key> struct hash {
    //void operator()() { std::cout << "hash<T>" << std::endl; }
//};

//__STL_TEMPLATE_NULL struct hash<char> {
    //void operator()() { std::cout << "hash<char>" << std::endl; }
//};

//__STL_TEMPLATE_NULL struct hash<unsigned char> {
    //void operator()() { std::cout << "hash<unsigned char>" << std::endl; }
//};

// 临时变量的使用
template <typename T>
class print
{
public:
    void operator()(const T &elem)
    { std::cout << elem << ' '; }
};


int main()
{
    // vector::swap
    vector<int> x, y;
    swap(x, y);
    iiiiiiiiii
    // 测试class template中拥有static data members
    std::cout << x._data << "|" << y._data << std::endl;
    iiiiiiiiii
    // vector::insert:测试class template内是否可以有template (members)
    int ia[5] = {0, 1, 2, 3, 4};
    vector<int> z;
    vector<int>::iterator ite;
    z.insert(ite, ia, ia+5);
    iiiiiiiiii
    // stack:测试是否可以根据前一个template参数而设定默认值
    stack<int> st;
    iiiiiiiiii
    // 测试class template可否拥有non-type template参数
    std::cout << deque<int>::iterator::buffer_size() << std::endl;
    std::cout << deque<int, alloc, 64>::iterator::buffer_size() << std::endl;
    iiiiiiiiii
    // bound friend templates
    stack<int> xx, yy;
    std::cout << (xx == yy) << std::endl;
    std::cout << (xx < yy) << std::endl;
    stack<char> yl;
    iiiiiiiiii
    //std::cout << (xx == yl) << std::endl; // 没有匹配项
    // 临时变量的使用
    std::vector<int> iv(ia, ia + 5);
    std::for_each(iv.begin(), iv.end(), print<int>()); // 它是一个临时对象，不是函数调用
    std::cout << '\n';
    iiiiiiiiii
    return 0;
}
