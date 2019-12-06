#ifndef __PRIORITY_QUEUE_HPP__
#define __PRIORITY_QUEUE_HPP__

#include "algorithm.hpp"
#include "vector.hpp"

namespace ich
{


// container dapter
template <class T, class Sequence = vector<T>, class Compare = less<typename Sequence::value_type>>
class priority_queue
{
public:
    typedef typename Sequence::value_type       value_type;
    typedef typename Sequence::size_type        size_type;
    typedef typename Sequence::reference        reference;
    typedef typename Sequence::const_reference  const_reference;
    priority_queue() : c() {}
    explicit priority_queue(const Compare &x) : c(), comp(x) {}

    template <class InputIterator>
    priority_queue(InputIterator first, InputIterator last, const Compare &x): c(first, last), comp(x) {
        make_heap(c.begin(), c.end(), comp);
    }

    template <class InputIterator>
    priority_queue(InputIterator first, InputIterator last) : c(first, last) {
        make_heap(c.begin(), c.end(), comp);
    }

    bool empty() { return c.empty(); }
    size_type size() { return c.size(); }
    const_reference top() { return c.front(); }
    void push(const value_type &x) {
        // __STL_TRY {} __STL_UNWIND(c.clear())
        c.push_back(x);
        push_heap(c.begin(), c.end(), comp);
    }
    void pop() {
        // __STL_TRY {} __STL_UNWIND(c.clear());
        pop_heap(c.begin(), c.end(), comp);
        c.pop_back();
    }
protected:
    Sequence c;
    Compare comp;
};


}


#endif
