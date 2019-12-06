// 前闭后开区间表示法[)
// 技巧：函数返回回调函数的返回值
#include <iostream>

template <class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last, const T &value) {
    while (first != last && *first != value) ++first;
    return first;
}

template <class InputIterator, class Function>
Function for_each(InputIterator first, InputIterator last, Function f) {
    for (; first != last; ++first)
        f(*first);
    return f;
}

int call(int s)
{
    std::cout << s << ' ';
    return s;
}

int main()
{
    int ia[5] = {0, 1, 2, 3, 4};
    int *it = ia;
    for_each(it, it+5, call);
    return 0;
}
