#include "vector.hpp"
#include "list.hpp"
#include "deque.hpp"
#include "stack.hpp"
#include "queue.hpp"
#include "priority_queue.hpp"
#include "slist.hpp"
#include "rb_tree.hpp"
#include "pair.hpp"
#include "set.hpp"
//#include "map.hpp"
//#include "multimap.hpp"
#include "multiset.hpp"
#include "hashtable.hpp"
//#include "hash_set.hpp"
//#include "hash_map.hpp"
//#include "hash_multiset.hpp"
//#include "hash_multimap.hpp"
//#include "../allocator/allocator.hpp"
#include "algorithm.hpp"
#include "hash_fun.hpp"
#include <string>
#include <cstring>

void vecotr_test() {
    // vector
    ich::vector<int> iv(2, 9);
    std::cout << iv.size() << std::endl;
    std::cout << iv.capacity() << std::endl;
    iv.push_back(2);
    iv.push_back(3);
    iv.push_back(4);
    iv.push_back(5);
    std::cout << iv.size() << iv.capacity() << std::endl;
    iv.pop_back();
    for (ich::vector<int>::iterator first = iv.begin(); first != iv.end(); ++first)
        std::cout << *first << ' ';
    std::cout << std::endl;
    std::cout << iv.size() << iv.capacity() << std::endl;
    std::cout << ich::distance(iv.begin(), iv.end()) << std::endl;
}

void list_test() {
    // list test
    int i;
    ich::list<int> ilist;
    std::cout << "size=" << ilist.size() << std::endl;
    ilist.push_back(0);
    ilist.push_back(1);
    ilist.push_back(2);
    ilist.push_back(3);
    ilist.push_back(4);
    std::cout << "size=" << ilist.size() << std::endl;
    ich::list<int>::iterator ite;
    for (ite = ilist.begin(); ite != ilist.end(); ++ite)
        std::cout << *ite << ' ';
    std::cout << std::endl;
    ite = ich::find(ilist.begin(), ilist.end(), 3);
    if (ite != 0)
        ilist.insert(ite, 99);
    std::cout << "size=" << ilist.size() << std::endl;
    std::cout << *ite << std::endl;
    for (ite = ilist.begin(); ite != ilist.end(); ++ite)
        std::cout << *ite << ' ';
    std::cout << std::endl;
    ite = ich::find(ilist.begin(), ilist.end(), 1);
    if (ite != 0)
        std::cout << *(ilist.erase(ite)) << std::endl;
    for (ite = ilist.begin(); ite != ilist.end(); ++ite)
        std::cout << *ite << ' ';
    std::cout << std::endl;
    //ilist.sort();
}

void deque_test() {
    // deque test
    ich::deque<int, ich::allocator<int>, 8> ideq(20, 9);
    std::cout << "size=" << ideq.size() << std::endl;
    for (int i = 0; i < ideq.size(); ++i) ideq[i] = i;
    for (int i = 0; i < ideq.size(); ++i) std::cout << ideq[i] << ' '; std::cout << std::endl;
    for (int i = 0; i < 3; i++) ideq.push_back(i);
    for (int i = 0; i < ideq.size(); ++i) std::cout << ideq[i] << ' '; std::cout << std::endl;
    std::cout << "size=" << ideq.size() << std::endl;
    ideq.push_back(3);
    for (int i = 0; i < ideq.size(); ++i) std::cout << ideq[i] << ' '; std::cout << std::endl;
    std::cout << "size=" << ideq.size() << std::endl;
    ideq.push_front(99);
    for (int i = 0; i < ideq.size(); ++i) std::cout << ideq[i] << ' '; std::cout << std::endl;
    std::cout << "size=" << ideq.size() << std::endl;
    ideq.push_front(98);
    ideq.push_front(97);
    for (int i = 0; i < ideq.size(); ++i) std::cout << ideq[i] << ' '; std::cout << std::endl;
    std::cout << "size=" << ideq.size() << std::endl;

    //ich::deque<int, ich::allocator<int>, 32>::iterator itr;
    //itr = ich::find(ideq.begin(), ideq.end(), 99);
    //std::cout << *itr << std::endl;
    //std::cout << *(itr.cur) << std::endl;
}

void stack_test() {
    // stack
    ich::stack<int, ich::list<int>> istack;
    istack.push(1);
    istack.push(3);
    istack.push(5);
    istack.push(7);
    std::cout << istack.size() << std::endl;
    std::cout << istack.top() << std::endl;
    istack.pop(); std::cout << istack.top() << std::endl;
    istack.pop(); std::cout << istack.top() << std::endl;
    istack.pop(); std::cout << istack.top() << std::endl;
    std::cout << istack.size() << std::endl;
}

void queue_test() {
    // queue
    ich::queue<int, ich::list<int>> iqueue;
    iqueue.push(1);
    iqueue.push(3);
    iqueue.push(5);
    iqueue.push(7);
    std::cout << iqueue.size() << std::endl;
    std::cout << iqueue.front() << std::endl;
    iqueue.pop(); std::cout << iqueue.front() << std::endl;
    iqueue.pop(); std::cout << iqueue.front() << std::endl;
    iqueue.pop(); std::cout << iqueue.front() << std::endl;
    std::cout << iqueue.size() << std::endl;
}

void heap_test() {
    // heap 底层vector完成
    int ia[9] = { 0, 1, 2, 3, 4, 8, 9, 3, 5 };
    ich::vector<int> ivec;
    for (int i = 0; i < 9; ++i) ivec.push_back(ia[i]);
    for (int i = 0; i < ivec.size(); ++i) std::cout << ivec[i] << ' '; std::cout << std::endl;
    ich::make_heap(ivec.begin(), ivec.end());
    for (int i = 0; i < ivec.size(); ++i) std::cout << ivec[i] << ' '; std::cout << std::endl;
    ivec.push_back(7);
    ich::push_heap(ivec.begin(), ivec.end());
    for (int i = 0; i < ivec.size(); ++i) std::cout << ivec[i] << ' '; std::cout << std::endl;
    ich::pop_heap(ivec.begin(), ivec.end());
    std::cout << ivec.back() << std::endl;
    ivec.pop_back();    // 取走尾值
    for (int i = 0; i < ivec.size(); ++i) std::cout << ivec[i] << ' '; std::cout << std::endl;
    ich::sort_heap(ivec.begin(), ivec.end());
    for (int i = 0; i < ivec.size(); ++i) std::cout << ivec[i] << ' '; std::cout << std::endl;
}

void priority_queue_test() {
    // priority_queue
    int ia[9] = {0, 1, 2, 3, 4, 8, 9, 3, 5};
    //ich::priority_queue<int> ipq(ia, ia + 9); // default by less<>
    ich::priority_queue<int, ich::vector<int>, ich::greater<int>> ipq(ia, ia + 9);
    //ich::priority_queue<int, ich::vector<int>, ich::less<int>> ipq(ia, ia + 9);
    std::cout << "size=" << ipq.size() << std::endl;
    for (int i = 0; i < ipq.size(); ++i) std::cout << ipq.top() << std::endl; std::cout << std::endl;
    while (!ipq.empty()) { std::cout << ipq.top() << ' '; ipq.pop(); } std::cout << std::endl;
}

void slist_test() {
    // slist
    int i;
    ich::slist<int> islist;
    std::cout << "size=" << islist.size() << std::endl;
    islist.push_front(9);
    islist.push_front(1);
    islist.push_front(2);
    islist.push_front(3);
    islist.push_front(4);
    std::cout << "size=" << islist.size() << std::endl;
    ich::slist<int>::iterator ite = islist.begin();
    ich::slist<int>::iterator ite2 = islist.end();
    for (; ite != ite2; ++ite) std::cout << *ite << ' '; std::cout << std::endl;
    ite = ich::find(islist.begin(), islist.end(), 1);
    if (ite != 0) islist.insert(ite, 99);
    std::cout << "size=" << islist.size() << std::endl;
    std::cout << *ite << std::endl;
    ite = islist.begin();
    ite2 = islist.end();
    for (; ite != ite2; ++ite) std::cout << *ite << ' '; std::cout << std::endl;
    ite = find(islist.begin(), islist.end(), 3);
    if (ite != 0) std::cout << *(islist.erase(ite)) << std::endl;
    ite = islist.begin();
    ite2 = islist.end();
    for (; ite != ite2; ++ite) std::cout << *ite << ' '; std::cout << std::endl;
    islist.clear();
    std::cout << islist.size() << std::endl;
}

void rb_tree_test() {
    // rb_tree
    ich::rb_tree<int, int, ich::identity<int>, ich::less<int>> itree;
    ich::rb_tree<int, int, ich::identity<int>, ich::less<int>>::iterator ite1, ite2;
    std::cout << itree.size() << std::endl;
    itree.insert_unique(10);
    itree.insert_unique(7);
    itree.insert_unique(8);
    itree.insert_unique(15);
    itree.insert_unique(5);
    itree.insert_unique(6);
    itree.insert_unique(11);
    itree.insert_unique(13);
    itree.insert_unique(12);
    std::cout << itree.size() << std::endl;
    ite1 = itree.begin();
    ite2 = itree.end();
    std::cout << *ite1 << std::endl;
    for (; ite1 != ite2; ++ite1) std::cout << *ite1 << ' '; std::cout << std::endl;
    // 测试颜色和operator++
    ite1 = itree.begin();
    ite2 = itree.end();
    ich::__rb_tree_base_iterator rbtite;
    for (; ite1 != ite2; ++ite1) {
        rbtite = ich::__rb_tree_base_iterator(ite1);    // 向上转型
        std::cout << *ite1 << '(' << rbtite.node->color << ") ";
    }
    std::cout << std::endl;
    ich::rb_tree<int, int, ich::identity<int>, ich::less<int>>::iterator ite3;
    ite3 = itree.find(11);
    std::cout << *ite3 << std::endl;
}

void set_test() {
    // set
    int i;
    int ia[5] = {0, 1, 2, 3, 4};
    ich::set<int> iset(ia, ia + 5);
    std::cout << "size=" << iset.size() << std::endl; std::cout << "3 count=" << iset.count(3) << std::endl;
    iset.insert(3);
    std::cout << "size=" << iset.size() << std::endl; std::cout << "3 count=" << iset.count(3) << std::endl;
    iset.insert(5);
    std::cout << "size=" << iset.size() << std::endl; std::cout << "3 count=" << iset.count(3) << std::endl;
    //iset.erase(1);
    //std::cout << "size=" << iset.size() << std::endl; std::cout << "3 count=" << iset.count(3) << std::endl;
    //std::cout << "1 count=" << iset.count(1) << std::endl;
    ich::set<int>::iterator ite1 = iset.begin();
    ich::set<int>::iterator ite2 = iset.end();
    for (; ite1 != ite2; ++ite1) std::cout << *ite1; std::cout << std::endl;

    //ite1 = std::find(iset.begin(), iset.end(), 3);
    //if (ite1 != iset.end()) std::cout << "3 found" << std::endl;
    //ite1 = std::find(iset.begin(), iset.end(), 1);
    //if (ite1 != iset.end()) std::cout << "1 not found" << std::endl;

    ite1 = iset.find(3);
    if (ite1 != iset.end()) std::cout << "3 found" << std::endl;
    ite1 = iset.find(6);
    if (ite1 == iset.end()) std::cout << "1 not found" << std::endl;
    //*ite1 = 9;    // 迭代器进行修改是不允许的
}

void multiset_test() {
    // multiset
    int i;
    int ia[5] = {0, 1, 2, 3, 4};
    ich::multiset<int> iset(ia, ia + 5);
    std::cout << "size=" << iset.size() << std::endl; std::cout << "3 count=" << iset.count(3) << std::endl;
    iset.insert(3);
    std::cout << "size=" << iset.size() << std::endl; std::cout << "3 count=" << iset.count(3) << std::endl;
    iset.insert(5);
    std::cout << "size=" << iset.size() << std::endl; std::cout << "3 count=" << iset.count(3) << std::endl;
    iset.erase(1);
    std::cout << "size=" << iset.size() << std::endl; std::cout << "3 count=" << iset.count(3) << std::endl;
    std::cout << "1 count=" << iset.count(1) << std::endl;
    ich::multiset<int>::iterator ite1 = iset.begin();
    ich::multiset<int>::iterator ite2 = iset.end();
    for (; ite1 != ite2; ++ite1) std::cout << *ite1; std::cout << std::endl;

    //ite1 = std::find(iset.begin(), iset.end(), 3);
    //if (ite1 != iset.end()) std::cout << "3 found" << std::endl;
    //ite1 = std::find(iset.begin(), iset.end(), 1);
    //if (ite1 != iset.end()) std::cout << "1 not found" << std::endl;

    ite1 = iset.find(3);
    if (ite1 != iset.end()) std::cout << "3 found" << std::endl;
    ite1 = iset.find(6);
    if (ite1 == iset.end()) std::cout << "1 not found" << std::endl;
    //*ite1 = 9;    // 迭代器修改是允许的
}

void map_n_multimap_test() {
    // map/multimap(invalid)
    //ich::map<std::string, int> simap;
    //simap[std::string("jihou")] = 1;    // ("jihou", 1)
    //simap[std::string("jerry")] = 2;    // ("jerry", 2)
    //simap[std::string("jason")] = 3;    // ("jason", 3)
    //simap[std::string("jimmy")] = 4;    // ("jimmy", 4)
    //ich::pair<std::string, int> value(std::string("david"), 5);
    //simap.insert(value);
    //ich::map<std::string, int>::iterator simap_iter = simap.begin();
    //for (; simap_iter != simap.end(); ++simap_iter)
        //std::cout << simap_iter->first << ' ' << simap_iter->second << std::endl;
    //// david 5
    //// jason 3
    //// jerry 2
    //// jimmy 4
    //// jjhou 1
    //int number = simap[string("jihou")];
    //std::cout << number << std::endl;
    //ich::map<string, int>::iterator ite1;
    //ite1 = simap.find(std::string("mchen"));
    //if (ite1 == simap.end()) std::cout << "mchen not found" << std::endl;   // mchen not found
    //ite1 = simap.find(std::string("jerry"));
    //if (ite1 != simap.end()) std::cout << "jerry found" << std::endl;       // jerry found
    //ite1->second = 9;
    //int number2 = simap[std::string("jerry")];
    //std::cout << number2 << std::endl;  // 9
}

void hashtable_test() {
    // hashtable
    ich::hashtable<int, int, ich::hash<int>, ich::identity<int>, ich::equal_to<int>, ich::alloc> iht(50, ich::hash<int>(), ich::equal_to<int>());
    std::cout << iht.size() << std::endl; std::cout << iht.bucket_count() << std::endl; std::cout << iht.max_bucket_count() << std::endl;
    iht.insert_unique(59);
    iht.insert_unique(63);
    iht.insert_unique(108);
    iht.insert_unique(2);
    iht.insert_unique(53);
    iht.insert_unique(55);
    iht.insert_equal(59);
    std::cout << iht.size() << std::endl;

    ich::hashtable<int, int, ich::hash<int>, ich::identity<int>, ich::equal_to<int>, ich::alloc>::iterator ite = iht.begin();
    for (int i = 0; i < iht.size(); ++i, ++ite) std::cout << *ite << ' '; std::cout << std::endl;
    for (int i = 0; i < iht.bucket_count(); ++i) {
        int n = iht.elems_in_bucket(i);
        if (n != 0) std::cout << "bucket[" << i << "] has " << n << " elems." << std::endl;
    }
    //for (int i = 0; i <= 47; i++)
        //iht.insert_equal(i);
    //std::cout << iht.size() << std::endl; std::cout << iht.bucket_count() << std::endl;
    for (int i = 0; i < iht.bucket_count(); ++i) {
        int n = iht.elems_in_bucket(i);
        if (n != 0) std::cout << "bucket[" << i << "] has " << n << " elems." << std::endl;
    }

    ite = iht.begin();
    for (int i = 0; i < iht.size(); ++i, ++ite) std::cout << *ite << ' '; std::cout << std::endl;
    std::cout << *(iht.find(2)) << std::endl;
    std::cout << iht.count(2) << std::endl;
}

//struct eqstr
//{
    //bool operator()(const char *s1, const char *s2) const { return strcmp(s1, s2) == 0; }
//};
//void lookup(const ich::hash_set<const char *, ich::hash<const char *>, eqstr> &Set, const char *word) {
    //ich::hash_set<const char *, ich::hash<const char *>, eqstr>::const_iterator it = Set.find(word);
    //std::cout << " " << word << ": " << (it != Set.end() ? "present" : "not present") << std::endl;
//}

void hashset_test() {
    // hashset
}


int main()
{
    std::cin.sync_with_stdio(false);
    std::cin.tie(nullptr);

    //vecotr_test();
    //list_test();
    //deque_test();
    //stack_test();
    //queue_test();
    //heap_test();
    //priority_queue_test();
    //slist_test();
    //rb_tree_test();
    //set_test();
    //multiset_test();
    //hashtable_test();
    hashset_test();

    return 0;
}
