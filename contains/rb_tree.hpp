#ifndef __RB_TREE_HPP__
#define __RB_TREE_HPP__

#include "algorithm.hpp"
#include "../allocator/allocator.hpp"
#include "../allocator/constructor.hpp"
#include "pair.hpp"

namespace ich
{



typedef bool __rb_tree_color_type;
const __rb_tree_color_type __rb_tree_red    = false;// 红色为0
const __rb_tree_color_type __rb_tree_black  = true; // 黑色为1

struct __rb_tree_node_base
{
    typedef __rb_tree_color_type color_type;
    typedef __rb_tree_node_base* base_ptr;
    // 有红黑两色,左右两节点
    color_type  color;
    base_ptr    parent;
    base_ptr    left;
    base_ptr    right;
    // 寻找极值
    static base_ptr minimum(base_ptr x) { while (x->left != 0) x = x->left; return x; }
    static base_ptr maximum(base_ptr x) { while (x->right != 0) x = x->right; return x; }
};

template <class Value>
struct __rb_tree_node : public __rb_tree_node_base
{
    typedef __rb_tree_node<Value>* link_type;
    Value value_field; // 节点值
};

// 节点双层结构和迭代器双层结构架构
struct __rb_tree_base_iterator
{
    typedef __rb_tree_node_base::base_ptr   base_ptr;
    typedef bidirectional_iterator_tag      iterator_category;
    typedef ptrdiff_t                       difference_type;

    base_ptr node;

    void increment() {
        if (node->right != 0) { // 如果有右节点，先右后直左取值
            node = node->right;
            while (node->left != 0)
                node = node->left;
        } else {    // 如果没有右节点，上溯父节点，直到所在节点不是右子节点，则y就是较大值
            base_ptr y = node->parent;
            while (node == y->right) {
                node = y;
                y = y->parent;
            }
            if (node->right != y)   // 没有根节点的右节点，特殊情况
                node = y;
        }
    }

    void decrement() {
        // header：红节点并且父节点的父节点是本身，指向max值
        if (node->color == __rb_tree_red && node->parent->parent == node) {
            node = node->right;
        } else if (node->left != 0) {   // 有左节点，先左后直右取值
            base_ptr y = node->left;
            while (y->right != 0)
                y = y->right;
        } else {    // 找出父节点，直到所在节点不是左子节点，y就是较小值
            base_ptr y = node->parent;
            while (node == y->left) {
                node = y;
                y = y->parent;
            }
            node = y;
        }
    }
};

template <class Value, class Ref, class Ptr>
struct __rb_tree_iterator : public __rb_tree_base_iterator
{
    typedef Value   value_type;
    typedef Ref     reference;
    typedef Ptr     pointer;
    typedef __rb_tree_iterator<Value, Value&, Value*>               iterator;
    typedef __rb_tree_iterator<Value, const Value&, const Value*>   const_iterator;
    typedef __rb_tree_iterator<Value, Ref, Ptr>                     self;
    typedef __rb_tree_node<Value>*                                  link_type;

    __rb_tree_iterator() {}
    __rb_tree_iterator(link_type x) { node = x; }
    __rb_tree_iterator(const iterator &it) { node = it.node; }

    reference operator*() const { return link_type(node)->value_field; }
    pointer operator->() const { return &(operator*()); }
    self& operator++() { increment(); return *this; }
    self operator++(int) {
        self tmp = *this;
        increment();
        return tmp;
    }
    self& operator--() { decrement(); return *this; }
    self operator--(int) {
        self tmp = *this;
        decrement();
        return tmp;
    }
    bool operator==(const self &x) { return node == x.node; }
    bool operator!=(const self &x) { return !(*this == x); }
};

// rb-tree数据结构
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc = alloc>
class rb_tree
{
protected:
    typedef void*                   void_pointer;
    typedef __rb_tree_node_base*    base_ptr;
    typedef __rb_tree_node<Value>   rb_tree_node;
    typedef allocator<rb_tree_node> rb_tree_node_allocator;
    typedef __rb_tree_color_type    color_type;
public:
    typedef Key                 key_type;
    typedef Value               value_type;
    typedef value_type*         pointer;
    typedef const value_type*   const_pointer;
    typedef value_type&         reference;
    typedef const value_type&   const_reference;
    typedef rb_tree_node*       link_type;
    typedef size_t              size_type;
    typedef ptrdiff_t           difference_type;
    typedef __rb_tree_iterator<value_type, reference, pointer> iterator;
    typedef __rb_tree_iterator<value_type, const_reference, const_pointer> const_iterator;

    rb_tree(const Compare &comp = Compare()) : node_count(0), key_compare(comp) { init(); }
    /* rb_tree(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc &x) : node_count(0), key_compare(x.key_compare); */
    ~rb_tree() { clear(); put_node(header); }
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
        operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &x);

    Compare key_comp() const { return key_compare; }
    iterator begin() { return leftmost(); }
    const_iterator cbegin() const { return leftmost(); }
    iterator end() const { return header; }
    const_iterator cend() const { return header; }
    bool empty() const { return node_count == 0; }
    size_type count(const key_type &x) const;
    size_type size() const { return node_count; }
    size_type max_size() const { return size_type(-1); }
    size_type erase(const Key &x);
    void erase(const Key *first, const Key *last);
    void erase(iterator position);
    void erase(iterator first, iterator last);
    void clear() {
        if (node_count != 0) {
            __erase(root());
            leftmost() = header;
            root() = 0;
            rightmost() = header;
            node_count = 0;
        }
    }
    // 将x插入rb-tree中，值唯一
    pair<iterator, bool> insert_unique(const value_type &x);
    /* iterator insert_unique(iterator position, const vlaue_type &x); */
    template <class II>
    void insert_unique(II first, II last);
    // 将x插入到rb-tree中，值可重复
    iterator insert_equal(const value_type &x);
    template <class II>
    void insert_equal(II first, II last);
    /* iterator insert_equal(iterator position, const value_type &x); */
    iterator find(const Key &k) const;
    //const_iterator cfind(const Key &k) const;
    iterator lower_bound(const Key &k);
    const_iterator lower_bound(const Key &k) const;
    iterator upper_bound(const Key &k);
    const_iterator upper_bound(const Key &k) const;
    pair<iterator, iterator> equal_range(const Key &k) { return pair<iterator, iterator>(lower_bound(k), upper_bound(k)); }
    pair<const_iterator, const_iterator> equal_range(const Key &k) const { return pair<const_iterator, const_iterator>(lower_bound(k), upper_bound(k));  }
private:
    iterator __insert(base_ptr x, base_ptr y, const value_type &v);
    link_type __copy(link_type x, link_type p);
    void __erase(link_type x) {
        while (x != 0) {
            __erase(right(x));
            link_type y = left(x);
            destroy_node(x);
            x = y;
        }
    }
    // ??????
    __rb_tree_node_base* __rb_tree_rebalance_for_erase(__rb_tree_node_base* z,
                                                    __rb_tree_node_base*& root,
                                                    __rb_tree_node_base*& leftmost,
                                                    __rb_tree_node_base*& rightmost);
    void init() {
        header = get_node();
        color(header) = __rb_tree_red;  // 令header为红色
        root() = 0;
        leftmost() = header;
        rightmost() = header;
    }
protected:
    size_type   node_count; // 节点数量
    link_type   header;
    Compare     key_compare;// 键值比较规则
    // 用来方便取得header成员
    link_type& root() const { return (link_type&) header->parent; }
    link_type& leftmost() const { return (link_type&) header->left; }
    link_type& rightmost() const { return (link_type&) header->right; }
    // 用来取得x节点成员
    static link_type& left(link_type x) { return (link_type&)(x->left); }
    static link_type& right(link_type x) { return (link_type&)(x->right); }
    static link_type& parent(link_type x) { return (link_type&)(x->parent); }
    static reference value(link_type x) { return x->value_field; }
    static const Key& key(link_type x) { return KeyOfValue()(value(x)); }
    static color_type& color(link_type x) { return (color_type&)(x->color); }
    // 方便取得x节点成员
     static link_type& left(base_ptr x) { return (link_type&)(x->left); }
    static link_type& right(base_ptr x) { return (link_type&)(x->right); }
    static link_type& parent(base_ptr x) { return (link_type&)(x->parent); }
    static reference value(base_ptr x) { return link_type(x)->value_field; }
    static const Key& key(base_ptr x) { return KeyOfValue()(value(x)); }
    static color_type& color(base_ptr x) { return (color_type&)(x->color); }
    // 求极大值和极小值
    static link_type minimum(link_type x) { return (link_type) __rb_tree_node_base::minimum(x); }
    static link_type maximum(link_type x) { return (link_type) __rb_tree_node_base::maximum(x); }

    link_type get_node() { return rb_tree_node_allocator::allocate(); }
    void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }

    link_type create_node(const value_type &x) {
        link_type tmp = get_node();
        // _STL_TRY
        construct(&tmp->value_field, x);
        return tmp;
    }
    // 复制一个节点
    link_type clone(link_type x) {
        link_type tmp = create_node(x->value_field);
        tmp->color = x->color;
        tmp->left = 0;
        tmp->right = 0;
        return tmp;
    }

    void destroy_node(link_type p) {
        destroy(&p->value_field);
        put_node(p);
    }
};

// 插入新值（允许重复）
// 返回值是rb-tree迭代器
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const Value &v) {
    link_type y = header;
    link_type x = root();
    while (x != 0) {
        y = x;
        x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
    }
    // x为新值插入点，y为插入点父节点，v为新值
    return __insert(x, y, v);
}

// extra
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
template <class II>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(II first, II last) {
    for (; first != last; ++first)
        insert_equal(*first);
}

// 插入新值（值唯一）
// 返回值是pair，first元素是rb-tree迭代器，second元素表示插入成功与否
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const Value &v) {
    link_type y = header;
    link_type x = root();
    bool comp = true;
    while (x != 0) {
        y = x;
        // 例如greater规则是大往左，小等往右。begin()最左也是最大值
        comp = key_compare(KeyOfValue()(v), key(x));
        x = comp ? left(x) : right(x);
    }
    iterator j = iterator(y);   // j也是x父节点
    // 如果离开while时comp为真，将插入左侧
    // 按照上面的规则，这里的意思是，寻找v的前一个位置
    // 在comp真时进入左侧时，j作为v的父节点比v的值小，因此比j大的值就是j的父节点--j
    // 如果j为begin()即最大值，是找不到比v大一级的节点的
    if (comp)
        if (j == begin())   // 如果插入点父节点为最左节点
            return pair<iterator, bool>(__insert(x, y, v), true);
        else    // 否则比对小一值，即父节点
            --j;
    // 如果键值不重复，执行插入程序(即j必须大于v)
    if (key_compare(key(j.node), KeyOfValue()(v)))
        return pair<iterator, bool>(__insert(x, y, v), true);
    // 发现重复，不插入新值
    return pair<iterator, bool>(j, false);
}

// extra
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
template <class II>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(II first, II last) {
    for (; first != last; ++first)
        insert_unique(*first);
}

// 插入执行程序
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__insert(base_ptr x_, base_ptr y_, const Value &v) {
    link_type x = (link_type) x_;
    link_type y = (link_type) y_;
    link_type z;

    z = create_node(v);
    // 放左还是右
    if (y == header || x != 0 || key_compare(KeyOfValue()(v), key(y))) {
        left(y) = z;                    // 首个节点left(y)为header->left，因此默认更新了leftmost()
        if (y == header) {
            root() = z;
            rightmost() = z;            // 第一个节点这里只需要更新rightmost()
        } else if (y == leftmost()) {   // 极左永远是leftmost()
            leftmost() = z;
        }
    } else {
        right(y) = z;
        if (y == rightmost())           // 极右永远是rightmost()
            rightmost() = z;
    }
    parent(z) = y;
    left(z) = 0;
    right(z) = 0;
    // 设置完节点后调整rb-tree
    __rb_tree_rebalance(z, header->parent);
    ++node_count;
    return iterator(z);
}

inline void __rb_tree_rotate_left(__rb_tree_node_base *x, __rb_tree_node_base *&root) {
    // x旋左为y的左，y上升为x的父，替代x
    // y的左为x的右
    __rb_tree_node_base *y = x->right;
    x->right = y->left;
    if (y->left != 0)
        y->left->parent = x;
    y->parent = x->parent;
    if (x == root)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;

}

inline void __rb_tree_rotate_right(__rb_tree_node_base *x, __rb_tree_node_base *&root) {
    // x旋右为y的右，y上升为x的父，替代x
    // y的右为x的左
    __rb_tree_node_base *y = x->left;
    x->left = y->right;
    if (y->right != 0)
        y->right->parent = x;
    y->parent = x->parent;
    if (x == root)
        root = y;
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;
    y->right = x;
    x->parent = y;
}

// 一些全局函数
// 调整rb-tree（旋转及改变颜色）
inline void __rb_tree_rebalance(__rb_tree_node_base *x, __rb_tree_node_base *&root) {
    x->color = __rb_tree_red;   // 新节点为红色
    while (x != root && x->parent->color == __rb_tree_red) {// 如果x的父为红色，
        if (x->parent == x->parent->parent->left) {         // 如果x的父为祖的左
            __rb_tree_node_base *y = x->parent->parent->right;  // 令y为x的伯
            if (y && y->color == __rb_tree_red) {           // 如果伯存在且为红色
                x->parent->color = __rb_tree_black;         // 令x父和伯为黑色
                y->color = __rb_tree_black;
                x->parent->parent->color = __rb_tree_red;   // 令x的祖为红色
                x = x->parent->parent;
            } else {                                        // 如果没有伯或为黑色
                if (x == x->parent->right) {                // 如果x为父的右
                    x = x->parent;
                    __rb_tree_rotate_left(x, root);         // 令x上升为父，并左旋
                }
                // x为parent的左（parent可能发生更改)
                x->parent->color = __rb_tree_black;         // 令x的父为黑色
                x->parent->parent->color = __rb_tree_red;   // 令x的祖为红色
                __rb_tree_rotate_right(x->parent->parent, root); // 以x的祖，右旋
            }
        } else {                                            // 如果x的父为祖的右（以下同上，只是相反）
            __rb_tree_node_base *y = x->parent->parent->left;   // 令y为x伯
            if (y && y->color == __rb_tree_red) {           // 如果伯存在且为红色，则x父和伯为黑色节点
                x->parent->color = __rb_tree_black;
                x->color = __rb_tree_black;
                x->parent->parent->color = __rb_tree_red;
                x = x->parent->parent;
            } else {                                        // 如果没有伯或为黑色，[[左节点]右旋]，[右节点]左旋
                if (x == x->parent->left) {
                    x = x->parent;
                    __rb_tree_rotate_right(x, root);
                }
                x->parent->color = __rb_tree_black;
                x->parent->parent->color = __rb_tree_red;
                __rb_tree_rotate_left(x->parent->parent, root);
            }
        }
    }
    root->color = __rb_tree_black;  // 不断向上调整，最后根节点永远为黑
}

// wtf
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
__rb_tree_node_base* rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__rb_tree_rebalance_for_erase(__rb_tree_node_base* z, __rb_tree_node_base*& root, __rb_tree_node_base*& leftmost, __rb_tree_node_base*& rightmost) {
    __rb_tree_node_base* y = z;
    __rb_tree_node_base* x = 0;
    __rb_tree_node_base* x_parent = 0;
    if (y->left == 0)
        x = y->right;
    else
        if (y->right == 0) x = y->left;
        else {
            y = y->right;
            while (y->left != 0) y = y->left;
            x = y->right;
        }

    if (y != z) {
        z->left->parent = y;
        y->left = z->left;
        if (y != z->right) { x_parent = y->parent;
            if (x) x->parent = y->parent;
            y->parent->left = x;
            y->right = z->right;
            z->right->parent = y;
        }
        else x_parent = y;
        if (root == z) root = y;
        else if (z->parent->left == z) z->parent->left = y;
        else z->parent->right = y;
        y->parent = z->parent;
        ich::swap(y->color, z->color);
        y = z;
    } else {
        x_parent = y->parent;
        if (x) x->parent = y->parent;
        if (root == z) root = x;
        else
            if (z->parent->left == z) z->parent->left = x;
            else z->parent->right = x;
        if (leftmost == z)
            if (z->right == 0) leftmost = z->parent;
            else leftmost = __rb_tree_node_base::minimum(x);
        if (rightmost == z)
            if (z->left == 0) rightmost = z->parent;
            else rightmost = __rb_tree_node_base::maximum(x);
    }

    if (y->color != __rb_tree_red) {
        while (x != root && (x == 0 || x->color == __rb_tree_black))
            if (x == x_parent->left) {
                __rb_tree_node_base* w = x_parent->right;
                if (w->color == __rb_tree_red) {
                    w->color = __rb_tree_black;
                    x_parent->color = __rb_tree_red;
                    __rb_tree_rotate_left(x_parent, root);
                    w = x_parent->right;
                }
                if ((w->left == 0 || w->left->color == __rb_tree_black) &&
                        (w->right == 0 || w->right->color == __rb_tree_black)) {
                    w->color = __rb_tree_red;
                    x = x_parent;
                    x_parent = x_parent->parent;
                } else {
                    if (w->right == 0 || w->right->color == __rb_tree_black) {
                        if (w->left) w->left->color = __rb_tree_black;
                        w->color = __rb_tree_red;
                        __rb_tree_rotate_right(w, root);
                        w = x_parent->right;
                    }
                    w->color = x_parent->color;
                    x_parent->color = __rb_tree_black;
                    if (w->right) w->right->color = __rb_tree_black;
                    __rb_tree_rotate_left(x_parent, root);
                    break;
                }
            } else {
                __rb_tree_node_base* w = x_parent->left;
                if (w->color == __rb_tree_red) {
                    w->color = __rb_tree_black;
                    x_parent->color = __rb_tree_red;
                    __rb_tree_rotate_right(x_parent, root);
                    w = x_parent->left;
                }
                if ((w->right == 0 || w->right->color == __rb_tree_black) &&
                        (w->left == 0 || w->left->color == __rb_tree_black)) {
                    w->color = __rb_tree_red;
                    x = x_parent;
                    x_parent = x_parent->parent;
                } else {
                    if (w->left == 0 || w->left->color == __rb_tree_black) {
                        if (w->right) w->right->color = __rb_tree_black;
                        w->color = __rb_tree_red;
                        __rb_tree_rotate_left(w, root);
                        w = x_parent->left;
                    }
                    w->color = x_parent->color;
                    x_parent->color = __rb_tree_black;
                    if (w->left) w->left->color = __rb_tree_black;
                    __rb_tree_rotate_right(x_parent, root);
                    break;
                }
            }
        if (x) x->color = __rb_tree_black;
    }
    return y;
}

// 寻找RB树中键值为k的节点
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key &k) const {
    link_type y = header;
    link_type x = root();
    while (x != 0)
        if (!key_compare(key(x), k))
            y = x, x = left(x);
        else
            x = right(x);
    iterator j = iterator(y);
    return (j == end() || key_compare(k, key(j.node))) ? end() : j;
}

//template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
//typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iteratcr
//rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::cfind(const Key &k) const {
    //link_type y = header;
    //link_type x = root();
    //while (x != 0)
        //if (!key_compare(key(x), k))
            //y = x, x = left(x);
        //else
            //x = right(x);
    //const_iterator j = const_iterator(y);
    //return (j == cend() || key_compare(k, key(j.node))) ? cend() : j;
//}


template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::lower_bound(const Key &k) {
    link_type y = header;
    link_type x = root();
    while (x != 0)
        if (!key_compare(key(x), k))
            y = x, x = left(x);
        else
            x = right(x);
    return iterator(y);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::lower_bound(const Key &k) const {
    link_type y = header;
    link_type x = root();
    while (x != 0)
        if (!key_compare(key(x), k))
            y = x, x = left(x);
        else
            x = right(x);
    return const_iterator(y);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::upper_bound(const Key &k) {
    link_type y = header;
    link_type x = root();
    while (x != 0)
        if (key_compare(k, key(x)))
            y = x, x = left(x);
        else
            x = right(x);
    return iterator(y);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::upper_bound(const Key &k) const {
    link_type y = header;
    link_type x = root();
    while (x != 0)
        if (key_compare(k, key(x)))
            y = x, x = left(x);
        else
            x = right(x);
    return const_iterator(y);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::count(const Key &k) const {
    pair<const_iterator, const_iterator> p = equal_range(k);
    size_type n = 0;
    distance(p.first, p.second, n);
    return n;
}

//template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
//inline pair<rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::terator>
//rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::equal_range(const Key &k) {
    //return pair<iterator, iterator>(lower_bound(k), upper_bound(k));
//}

//template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
//inline pair<rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::onst_iterator, rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::onst_iterator>
//rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::equal_range(const Key &k) {
    //return pair<const_iterator, const_iterator>(lower_bound(k), upper_bound(k));
//}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(const Key &x) {
    pair<iterator, iterator> p = equal_range(x);
    size_type n = 0;
    distance(p.first, p.second, n);
    erase(p.first, p.second);
    return n;
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator position) {
    link_type y = (link_type) __rb_tree_rebalance_for_erase(position.node, header->parent, header->left, header->right);
    destroy_node(y);
    --node_count;
}


template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator first, iterator last) {
    if (first == begin() && last == end())
        clear();
    else
        while (first != last) erase(first++);
}




}


#endif
