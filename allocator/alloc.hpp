#ifndef __ALLOC_HPP__
#define __ALLOC_HPP__

#define __NODE_ALLOCATOR_THREADS false
#ifdef __USE_MALLOC // 一级配置器
//typedef __malloc_alloc_template<0> malloc_alloc;
//typedef malloc_alloc alloc;
#define malloc_alloc    __malloc_alloc_template<0>
#define alloc           malloc_alloc
#else               // 二级配置器
//typedef __default_alloc_template<__NODE_ALLOCATOR_THREADS, 0> alloc;
#define alloc __default_alloc_template<__NODE_ALLOCATOR_THREADS, 0>
#endif

#if 0
#include <new>
#define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#include <new>
#include <cstdlib>
#include <iostream>
#define __THROW_BAD_ALLOC std::cerr << "out of memory" << std::endl; exit(1)
#endif

//template <class T, class Alloc>
//class simple_alloc {
//public:
    //static T* allocate(size_t n) { return 0 == n ? 0 : (T*) alloc::allocate(n * sizeof(T)); }
    //static T* allocate(void) { return (T*) alloc::allocate(sizeof(T)); }
    //static void deallocate(T *p, size_t n) { if (0 != n) alloc::deallocate(p, n * sizeof(T)); }
    //static void deallocate(T *p) { return alloc::deallocate(p, sizeof(T)); }
//};

// 一级配置器，当块大小小于128bytes都将调用此类
namespace ich {

template <int inst>
class __malloc_alloc_template
{
private:
    // 处理内存不足
    static void *oom_malloc(size_t);
    static void *oom_realloc(void *, size_t);
    static void (* __malloc_alloc_oom_handler)();
public:
    static void* allocate(size_t n) {
        void *result = malloc(n);
        if (0 == result) result = oom_malloc(n);
        return result;
    }

    static void deallocate(void *p, size_t /* n */) {
        free(p);
    }

    static void* reallocate(void *p, size_t /* old_sz */, size_t new_sz) {
        void *result = realloc(p, new_sz);
        if (0 == result) result = oom_realloc(p, new_sz);
        return result;
    }

    // 仿真C++的set_new_handler()
    // void (* handler) () = f
    // void (* f)() = a
    // void (* handler(void (* f)())) = f(a)
    static void (*set_malloc_handler(void (* f)())) ()
    {
        void (* old)() = __malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler = f;
        return (old);
    }
};

// static data members init
template <int inst>
void (* __malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

// 如果分配失败，不断重复分配
template <int inst>
void* __malloc_alloc_template<inst>::oom_malloc(size_t n) {
    void (* my_malloc_handler)();
    void *result;
    for (;;) {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == my_malloc_handler) { __THROW_BAD_ALLOC; }
        (*my_malloc_handler)();
        if (result = malloc(n)) return (result);
    }
}

// 如果分配失败，不断重复分配
template <int inst>
void* __malloc_alloc_template<inst>::oom_realloc(void *p, size_t n) {
    void (* my_malloc_handler)();
    void *result;
    for (;;) {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == my_malloc_handler) { __THROW_BAD_ALLOC; }
        (*my_malloc_handler)();
        if (result = realloc(p, n)) return (result);
    }
}

typedef __malloc_alloc_template<0> malloc_alloc;

// 二级配置器：当区块小雨128bytes，则以内存池（memory
// pool）管理，每次配置一大块内存，并维护链表，当有内
// 存需求则从free-lists中拨出，释放了则回收区块。内存
// 需求不为8的倍数将被上调

// 区块下限，也是对齐大小
enum { __ALIGN = 8 };
// 区块上限
enum { __MAX_BYTES = 128 };
// 能分配的free-lists个数
enum { __NFREELISTS = __MAX_BYTES / __ALIGN };

template <bool threads, int inst>
class __default_alloc_template
{
private:
    union obj {
        union obj *free_list_link;
        char client_data[1];    // 对客户可见的
    };
    // bytes上调为8的倍数
    static size_t round_up(size_t bytes) {
        return (((bytes) + __ALIGN - 1) & ~(__ALIGN - 1));
    }
    // 根据区块大小取第n个free-list的内存
    static size_t freelist_index(size_t bytes) {
        return (((bytes) + __ALIGN - 1) / __ALIGN - 1);
    }
    static void* refill(size_t n);
    static char* chunk_alloc(size_t size, int &nobjs);
    // 16个free_list，分别管理大小为8,16,24,32,40,48,56,64,72,80,88,96,104,112,120,128区块
    static obj *volatile free_list[__NFREELISTS];
    static char *start_free;    // 内存池开始位置
    static char *end_free;      // 内存池结束位置
    static size_t heap_size;
public:
    // 内存配置函数
    static void* allocate(size_t n) {
        obj *result;
        // 如果大于128就调用malloc（一级配置器）
        if (n > (size_t) __MAX_BYTES)
            return (malloc_alloc::allocate(n));
        // 寻找free list
        //my_free_list = free_list[freelist_index(n)];
        obj *volatile *my_free_list = free_list + freelist_index(n);
        if ((result = *my_free_list) == 0)
            return (void *) refill(round_up(n));
        *my_free_list = result->free_list_link;
        return (result);
    }
    // 内存释放函数
    static void deallocate(void *p, size_t n) {
        obj *q = (obj *) p;
        // 如果大于128就调用malloc（一级配置器）
        if (n > (size_t) __MAX_BYTES)
            return malloc_alloc::deallocate(p, n);  // return void
        obj *volatile *my_free_list = free_list + freelist_index(n);
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    }
    static void* reallocate(void *p, size_t old_sz, size_t new_sz);
};

// static data members init
template <bool threads, int inst>
char* __default_alloc_template<threads, inst>::start_free = 0;

template <bool threads, int inst>
char* __default_alloc_template<threads, inst>::end_free = 0;

template <bool threads, int inst>
size_t __default_alloc_template<threads, inst>::heap_size = 0;

template <bool threads, int inst>
typename __default_alloc_template<threads, inst>::obj *volatile
__default_alloc_template<threads, inst>::free_list[__NFREELISTS] =
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// 重新填充free lists，由chunk_alloc()取空间
template <bool threads, int inst>
void* __default_alloc_template<threads, inst>::refill(size_t n) {
    int nobjs = 20; // 默认区块，可能小于20
    char *chunk = chunk_alloc(n, nobjs);
    obj *current_obj, *next_obj;
    // 如果只获得一个区块，区块就分配给调用者
    if (1 == nobjs) return chunk;
    // 否则准备将free_list链成串
    obj *volatile *my_free_list = free_list + freelist_index(n);
    obj *result = (obj *) chunk;
    *my_free_list = next_obj = (obj *) (chunk + n);
    // 链成串，长度为nobjs-1
    for (int i = 1; ; i++) {    // 从1开始，因为第0个返回给客户（客户只需要n个，这里是在链n×20）
        current_obj = next_obj;
        next_obj = (obj *) ((char *) next_obj + n);
        // 假如分配有20，但会扩展出40，这里只处理前20，第一是需要的块，余下20留在内存池
        if (nobjs - 1 == i) {
            current_obj -> free_list_link = 0;
            break;
        } else {
            current_obj -> free_list_link = next_obj;
        }
    }
    return result;
}

// 内存池
// 好像默认是从else分支进入malloc出足够的空间后再递归调用自身完成串接
template <bool threads, int inst>
char* __default_alloc_template<threads, inst>::chunk_alloc(size_t size, int &nobjs) {
    char *result;
    size_t total_bytes = size * nobjs;          // 需要的内存总大小
    size_t bytes_left = end_free - start_free;  // 内存池剩余空间

    if (bytes_left >= total_bytes) {// 内存池剩余空间足够
        result = start_free;
        start_free += total_bytes;  // 内存池起始后移，与返回的内存块区域无关
        return result;
    } else if (bytes_left >= size) {// 内存池剩余部分空间
        nobjs = bytes_left / size;
        result = start_free;
        start_free += (size * nobjs);
        return result;
    } else {                        // 内存池空间连区块大小都没有:首先进入这里分配空间
        // 为bytes_to_get配置为需求量的两倍加上随配置次数增加而增加的附加量heap_size
        size_t bytes_to_get = 2 * total_bytes + round_up(heap_size >> 4);
        // 将内存池残余空间利用
        if (bytes_left > 0) {
            obj *volatile *my_free_list = free_list + freelist_index(bytes_left);
            ((obj*) start_free)->free_list_link = *my_free_list;
            *my_free_list = (obj*) start_free;
        }
        // 配置用来补充内存池的heap空间
        start_free = (char *) malloc (bytes_to_get);
        if (start_free == 0) {     // malloc空间不足
            obj *volatile *my_free_list, *p;
            for (int i = size; i <= __MAX_BYTES; i += __ALIGN) {
                my_free_list = free_list + freelist_index(i);
                if ((p = *my_free_list) != 0) {
                    *my_free_list = p->free_list_link;
                    start_free = (char *) p;
                    end_free = start_free + i;
                    return chunk_alloc(size, nobjs);    // 递归调用修正nobjs
                }
            }
            // 如果还是没内存：调用一级配置器
            end_free = 0;
            start_free = (char *) malloc_alloc::allocate(bytes_to_get);
        }
        heap_size += bytes_to_get;
        end_free = start_free + bytes_to_get;
        return chunk_alloc(size, nobjs);
    }
}

}

#endif
