#ifndef __PAIR_HPP__
#define __PAIR_HPP__


namespace ich
{


template <typename T1, typename T2>
struct pair
{
    typedef T1 first_type;
    typedef T2 second_type;
    T1 first;
    T2 second;
    pair() : first(T1()), second(T2()) {}
    pair(const T1 &_x, const T2 &_y) : first(_x), second(_y) {}
};

template <class T1, class T2>
inline bool operator==(const pair<T1, T2> &_x, const pair<T1, T2> &_y) {
    return _x.first == _y.first && _x.second == _y.seoncd;
}

template <class T1, class T2>
inline bool operator<(const pair<T1, T2> &_x, const pair<T1, T2> &_y) {
    return _x.first < _y.first || (!(_y.first < _x.first) && _x.second < _y.second);
}

template <class T1, class T2>
inline bool operator!=(const pair<T1, T2> &_x, const pair<T1, T2> &_y) {
    return !(_x == _y);
}

template <class T1, class T2>
inline bool operator>(const pair<T1, T2> &_x, const pair<T1, T2> &_y) {
    return _y < _x;
}

template <class T1, class T2>
inline bool operator<=(const pair<T1, T2> &_x, const pair<T1, T2> &_y) {
    return _y < _x;
}

template <class T1, class T2>
inline bool operator>=(const pair<T1, T2> &_x, const pair<T1, T2> &_y) {
    return !(_x < _y);
}

template <class T1, class T2>
inline pair<T1, T2> make_pair(const T1 &_x, const T2 &_y) {
    return pair<T1, T2>(_x, _y);
}


}


#endif
