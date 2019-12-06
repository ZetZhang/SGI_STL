#ifndef __QUEUE_HPP__
#define __QUEUE_HPP__

#include "deque.hpp"

namespace ich
{


// contain adapter
template <class T, class Sequence = deque<T>>
class queue {
private:
    template <class Ty, class Seq>
    friend bool operator== (const queue<Ty, Seq> &, const queue<Ty, Seq> &);
    template <class Ty, class Seq>
    friend bool operator< (const queue<Ty, Seq> &, const queue<Ty, Seq> &);
public:
    typedef typename Sequence::value_type       value_type;
    typedef typename Sequence::size_type        size_type;
    typedef typename Sequence::reference        reference;
    typedef typename Sequence::const_reference  const_reference;

    bool empty() const { return c.empty(); }
    size_type size() const { return c.size(); }
    reference front() { return c.front(); }
    const_reference cfront() const { return c.cfront(); }
    reference back() { return c.back(); }
    const_reference cback() const { return c.cback(); }
    void push(const value_type &x) { c.push_back(x); }
    void pop() { c.pop_front(); }
protected:
    Sequence c;
};

template <class T, class Sequence>
inline bool operator==(const queue<T, Sequence> &x, const queue<T, Sequence> &y) {
    return x.c == y.c;
}

template <class T, class Sequence>
inline bool operator<(const queue<T, Sequence> &x, const queue<T, Sequence> &y) {
    return x.c < y.c;
}

}


#endif
