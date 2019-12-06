#include "allocator.hpp"
#include "jj_allocator.hpp"
#include "uninitialized.hpp"
#include "constructor.hpp"
//#include "alloc.hpp"
#include <vector>
#include <iostream>

void none()
{

}

void what()
{
    std::cout << "fuck" << std::endl;
    return none();
}

template <class T>
struct MyIter {
    typedef T value_type;
    T *ptr;
    MyIter(T *p) : ptr(p) {}
    T& operator*() { return *ptr; }
};

template <class I>
struct iterator_traits {
    typedef typename I::value_type value_type;
};
// 偏特化萃取
template <class T>
struct iterator_traits<T*> {
    typedef T value_type;
};
template <class T>
struct iterator_traits<const T*> {
    typedef T value_type;
};

template <class T>
typename iterator_traits<T>::value_type func(T ite) {
    return *ite;
}

int main()
{
    int ia[6] = {0, 1, 2, 3, 4, 5};
    unsigned int i;
    // SGI STL的每个容器都默认在template声明指定缺省配置器为alloc
    // SGI STL还有个效率不佳的allocator配置器？？一般不用
    std::vector<int, ich::allocator<int>> iv(ia, ia+6);
    for (i=0; i < iv.size(); i++)
        std::cout << iv[i] << ' ';
    std::cout << std::endl;
    MyIter<int> it(new int(8));
    std::cout << func(it) << std::endl;
    return 0;
}
