#ifndef __STACK_HPP__
#define __STACK_HPP__

#include "deque.hpp"

namespace ich {

// container adapter
template <class T, class Sequence = deque<T>>
class stack {
private:
    template <class Ty, class Seq>
    friend bool operator== (const stack<Ty, Seq> &, const stack<Ty, Seq> &);
    template <class Ty, class Seq>
    friend bool operator< (const stack<Ty, Seq> &, const stack<Ty, Seq> &);
public:
    typedef typename Sequence::value_type       value_type;
    typedef typename Sequence::size_type        size_type;
    typedef typename Sequence::reference        reference;
    typedef typename Sequence::const_reference  const_reference;

    bool empty() const { return c.empty(); }
    size_type size() const { return c.size(); }
    reference top() { return c.back(); }
    const_reference ctop() const { return c.cback(); }
    void push(const value_type &x) { c.push_back(x); }
    void pop() { c.pop_back(); }
protected:
    Sequence c;
};

template <class T, class Sequence>
inline bool operator==(const stack<T, Sequence> &x, const stack<T, Sequence> &y) {
    return x.c == y.c;
}

template <class T, class Sequence>
inline bool operator<(const stack<T, Sequence> &x, const stack<T, Sequence> &y) {
    return x.c < y.c;
}


}

#endif
