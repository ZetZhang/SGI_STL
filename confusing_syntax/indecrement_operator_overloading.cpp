// 前置和后置的写法
#include <iostream>

class INT
{
    friend std::ostream& operator<<(std::ostream &os, const INT &i);
 public:
    INT(int i) : m_i(i) {};

    INT& operator++()           // ++INT
    {
        ++(this->m_i);
        return *this;
    }

    const INT operator++(int)   // INT++
    {
        INT temp = *this;
        ++(*this);              // call this:operator++()
        return temp;
    }

    INT& operator--()           // --INT
    {
        --(this->m_i);
        return *this;
    }

    const INT operator--(int)   // INT--
    {
        INT temp = *this;
        --(*this);              // call this:operator--()
        return temp;
    }


    int& operator*() const
    {
        return (int&)m_i;
    }

 private:
    int m_i;
};

std::ostream& operator<<(std::ostream &os, const INT &i)
{
    os << '[' << i.m_i << ']';
    return os;
}

int main()
{
    INT I(5);
    std::cout << I++;
    std::cout << ++I;
    std::cout << I--;
    std::cout << --I;
    std::cout << *I;
    return 0;
}
