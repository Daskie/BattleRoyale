#pragma once

#ifndef __GNUC__
#define USE_RPMALLOC
#endif


#include <scoped_allocator>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <cstdlib>


#ifdef USE_RPMALLOC



#include "ThirdParty/CoherentLabs_rpmalloc/rpmalloc.h"



// trick to call memory initialization functions before static evaluations
namespace detail {

struct InitializeMemory {

    InitializeMemory() {
        if (!initialized++) {
            coherent_rpmalloc::rpmalloc_initialize();
        }
    }

    static int initialized;

};

static InitializeMemory f_initializeMemory;

}



#endif



inline void * allocate(size_t size) {
#ifdef USE_RPMALLOC
    return coherent_rpmalloc::rpmalloc(size);
#else
    return std::malloc(size);
#endif
}

inline void deallocate(void * ptr) {
#ifdef USE_RPMALLOC
    return coherent_rpmalloc::rpfree(ptr);
#else
    return std::free(ptr);
#endif
}



template <typename T>
struct Allocator {

    template <typename U> friend struct Allocator;

    using value_type = T;
    using pointer = T *;

    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;

    Allocator() = default;

    ~Allocator() = default;

    template <typename U> Allocator(const Allocator<U> &) {}

    pointer allocate(std::size_t n) {
        return static_cast<pointer>(::allocate(n * sizeof(T)));
    }

    void deallocate(pointer p, std::size_t n) {
        ::deallocate(p);
    }

};

template <typename T1, typename T2>
bool operator==(const Allocator<T1> & a1, const Allocator<T2> & a2) {
    return true;
}

template <typename T1, typename T2>
bool operator!=(const Allocator<T1> & a1, const Allocator<T2> & a2) {
    return false;
}

#ifdef USE_RPMALLOC
template <typename T> using ScopedAllocator = std::scoped_allocator_adaptor<Allocator<T>>;
#else
template <typename T> using ScopedAllocator = std::allocator<T>;
#endif



// Standard containers using custom memory allocator

// std::string equivalent
using String = std::basic_string<char, std::char_traits<char>, ScopedAllocator<char>>;

#ifdef USE_RPMALLOC

// convert String to std::string
inline std::string convert(const String & string) {
    return std::string(string.c_str());
}

// convert std::string to String
inline String convert(const std::string & string) {
    return String(string.c_str());
}

#else

inline std::string convert(const std::string & string) {
    return string;
}

#endif

// std::vector equivalent
template <typename T>
using Vector = std::vector<T, ScopedAllocator<T>>;

// std::list equivalent
template <typename T>
using List = std::list<T, ScopedAllocator<T>>;

// std::set equivalent
template <typename K, typename E = std::less<K>>
using Set = std::set<K, E, ScopedAllocator<K>>;

// std::unordered_set equivalent
template <typename K, typename H = std::hash<K>, typename E = std::equal_to<K>>
using UnorderedSet = std::unordered_set<K, H, E, ScopedAllocator<K>>;

// std::map equivalent
template <typename K, typename V, typename E = std::less<K>>
using Map = std::map<K, V, E, ScopedAllocator<std::pair<K, V>>>;

// std::unordered_map equivalent
template <typename K, typename V, typename H = std::hash<K>, typename E = std::equal_to<K>>
using UnorderedMap = std::unordered_map<K, V, H, E, ScopedAllocator<std::pair<const K, V>>>;



// std::unique_ptr variant using custom memory allocator
template <typename T>
class UniquePtr {

    template <typename U> friend class UniquePtr;

    public:

    template <typename... Args> static UniquePtr<T> make(Args &&... args);

    template <typename SubT, typename... Args> static UniquePtr<T> makeAs(Args &&... args);

    public:

    UniquePtr();
    UniquePtr(const UniquePtr<T> & other) = delete;
    template <typename U, typename std::enable_if<std::is_base_of<T, U>::value, int>::type = 0>
    UniquePtr(UniquePtr<U> && other);

    ~UniquePtr();

    UniquePtr<T> & operator=(const UniquePtr<T> & other) = delete;
    UniquePtr<T> & operator=(UniquePtr<T> && other);

    explicit operator bool() const { return m_v; }

    void release();

    T * get() { return m_v; }
    const T * get() const { return m_v; }

    T & operator*() { return *m_v; }
    const T & operator*() const { return *m_v; }

    T * operator->() { return m_v; }
    const T * operator->() const { return m_v; }

    private:

    UniquePtr(T * v);

    private:

    T * m_v;

};



// std::unique_ptr array variant using custom memory allocator
template <typename T>
class UniquePtr<T[]> {

    public:

    static UniquePtr<T[]> make(size_t size);

    public:

    UniquePtr();
    UniquePtr(const UniquePtr<T[]> & other) = delete;
    UniquePtr(UniquePtr<T[]> && other);

    ~UniquePtr();

    UniquePtr<T[]> & operator=(const UniquePtr<T[]> & other) = delete;
    UniquePtr<T[]> & operator=(UniquePtr<T[]> && other);

    explicit operator bool() const { return m_vs; }

    void release();

    T * get() { return m_vs; }
    const T * get() const { return m_vs; }

    T & operator[](size_t i) { return m_vs[i]; };
    const T & operator[](size_t i) const { return m_vs[i]; };

    private:

    UniquePtr(T * vs);

    private:

    T * m_vs;

};



// TEMPLATE IMPLEMENTATION /////////////////////////////////////////////////////



template <typename T>
template <typename... Args>
UniquePtr<T> UniquePtr<T>::make(Args &&... args) {
    return UniquePtr<T>(new (allocate(sizeof(T))) T(std::forward<Args>(args)...));
}

template <typename T>
template <typename SubT, typename... Args>
UniquePtr<T> UniquePtr<T>::makeAs(Args &&... args) {
    static_assert(std::is_base_of<T, SubT>::value, "SubT must be derived from T");
    return UniquePtr<T>(new (allocate(sizeof(SubT))) SubT(std::forward<Args>(args)...));
}

template <typename T>
UniquePtr<T>::UniquePtr() :
    m_v(nullptr)
{}

template <typename T>
template <typename U, typename std::enable_if<std::is_base_of<T, U>::value, int>::type>
UniquePtr<T>::UniquePtr(UniquePtr<U> && other) :
    m_v(other.m_v)
{
    other.m_v = nullptr;
}

template <typename T>
UniquePtr<T>::~UniquePtr() {
    release();
}

template <typename T>
UniquePtr<T> & UniquePtr<T>::operator=(UniquePtr<T> && other) {
    m_v = other.m_v;
    other.m_v = nullptr;
    return *this;
}

template <typename T>
void UniquePtr<T>::release() {
    if (!m_v) {
        return;
    }

    m_v->~T();
    deallocate(m_v);
    m_v = nullptr;
}

template <typename T>
UniquePtr<T>::UniquePtr(T * v) :
    m_v(v)
{}



template <typename T>
UniquePtr<T[]> UniquePtr<T[]>::make(size_t size) {
    static_assert(std::is_default_constructible<T>::value, "T must be default constructible");
    unsigned char * mem(static_cast<unsigned char *>(allocate(sizeof(size_t) + size * sizeof(T))));
    *reinterpret_cast<size_t *>(mem) = size;
    T * arr(reinterpret_cast<T *>(mem + sizeof(size_t)));
    for (size_t i(0); i < size; ++i) new (arr + i) T();
    return UniquePtr<T[]>(arr);
}

template <typename T>
UniquePtr<T[]>::UniquePtr() :
    m_vs(nullptr)
{}

template <typename T>
UniquePtr<T[]>::UniquePtr(UniquePtr<T[]> && other) :
    m_vs(other.m_vs)
{
    other.m_vs = nullptr;
}

template <typename T>
UniquePtr<T[]>::~UniquePtr() {
    release();
}

template <typename T>
UniquePtr<T[]> & UniquePtr<T[]>::operator=(UniquePtr<T[]> && other) {
    m_vs = other.m_vs;
    other.m_vs = nullptr;
    return *this;
}

template <typename T>
void UniquePtr<T[]>::release() {
    if (!m_vs) {
        return;
    }

    unsigned char * mem(reinterpret_cast<unsigned char *>(m_vs) - sizeof(size_t));
    size_t size(*reinterpret_cast<size_t *>(mem));
    for (size_t i(0); i < size; ++i) m_vs[i].~T();
    deallocate(mem);
    m_vs = nullptr;
}

template <typename T>
UniquePtr<T[]>::UniquePtr(T * vs) :
    m_vs(vs)
{}