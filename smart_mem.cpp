#include <bits/stdc++.h>

template <typename T>
concept TriviallyCopyable = std::is_trivially_copyable<T>::value;

template <TriviallyCopyable T>
void safe_memcpy(T* dest, const T* src, size_t count)
{
    if (dest == nullptr || src == nullptr)
    {
        throw std::invalid_argument("Null pointer passed to safe_memcpy");
    }
    std::memcpy(dest, src, count * sizeof(T));
}

struct MyType 
{
    int m_val;
    MyType () = default;
    MyType(int val): m_val(val)
    {}
};

int main()
{
    int src[5] = {1, 2, 3, 4, 5};
    int dest[5];

    safe_memcpy(dest, src, 5);
    std::cout<< "destination arr: "<<  dest[0]<<", "<< std::addressof(dest)<<"\n";

    auto* memory = std::malloc(sizeof(MyType));
    auto* my_type_ptr = reinterpret_cast<MyType*>(memory);
    std::construct_at(my_type_ptr, MyType(1));
    std::destroy_at(my_type_ptr);
    std::free(memory);

    return 0;
}