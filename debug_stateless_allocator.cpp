#include <bits/stdc++.h>
template<typename T>
class myallocator
{
public:

    using value_type = T;
    using pointer = T *;
    using size_type = std::size_t;
    using is_always_equal = std::true_type;

public:

    myallocator()
    {
        std::cout << this << " constructor, sizeof(T): "
                  << sizeof(T) << '\n';
    }

    template <typename U>
    myallocator(const myallocator<U> &other) noexcept
    { 
        (void) other; 
    }

    myallocator(myallocator &&other) noexcept
    {
        (void) other;
        std::cout << this << " move constructor, sizeof(T): "<< sizeof(T) << '\n';
    }

    myallocator &operator=(myallocator &&other) noexcept
    {
        (void) other;
        std::cout << this << " move assignment, sizeof(T): "<< sizeof(T) << '\n';
        return *this;
    }

    myallocator(const myallocator &other) noexcept
    {
        (void) other;
        std::cout << this << " copy constructor, sizeof(T): "<< sizeof(T) << '\n';
    }

    myallocator &operator=(const myallocator &other) noexcept
    {
        (void) other;
        std::cout << this << " copy assignment, sizeof(T): "<< sizeof(T) << '\n';
        return *this;
    }

    pointer allocate(size_type n)
    {
        if (auto ptr = static_cast<pointer>(malloc(sizeof(T) * n))) 
        {
            std::cout << this << "  Alloc [" << n << "]: " << ptr << '\n';
            return ptr;
        }

        throw std::bad_alloc();
    }

    void deallocate(pointer ptr, size_type n)
    {
        (void) n;

        std::cout << this << "  Dealloc [" << n << "]: " << ptr << '\n';
        free(ptr);
    }
};

template <typename T1, typename T2>
bool operator==(const myallocator<T1> &, const myallocator<T2> &)
{ return true; }

template <typename T1, typename T2>
bool operator!=(const myallocator<T1> &, const myallocator<T2> &)
{ return false; }

int main()
{

    std::list<int, myallocator<int>> mylist1;
    std::list<int, myallocator<int>> mylist2;

    mylist1.emplace_back(42);
    mylist1.emplace_back(42);

    std::vector<int, myallocator<int>> tv;
    tv.emplace_back(1);
    tv.emplace_back(2);
    
    return 0;
}


