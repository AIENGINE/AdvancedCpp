#include <iostream>
#include <vector>
#include <tuple>
#include <initializer_list>
#include <boost/type_index.hpp>

#define ENABLE_BASIC_FORWARDING_EXAMPLE 0
#define ENABLE_TUPLE_EXAMPLE 1
using std::cout, std::endl, std::string;

#if ENABLE_BASIC_FORWARDING_EXAMPLE
void handleValue(const int&& value) { cout<<"Integer= "<< value<< endl; }
void handleValue(const double&& value) { cout<<"double= "<< value<< endl; }
void handleValue(const string&& value) { cout<<"String= "<< value<< endl; }
void handleValue(const int& value) { cout<<"Lvalue ref= "<<value<<endl; }

template <typename T>
void processValues(T&& arg) //base case for the recursive function
{
    handleValue(arg);
}

template <typename T, typename ...Ts>
void processValues(T&& arg, Ts&& ...args) //function that invokes recursion
//here also function parameters serve as a logic division for recursion case (base + loop or terminating condition and loop)
{
    cout<< "original arg= "<<arg<< ": "<<"args left="<<sizeof...(args)<<endl;
    handleValue(std::forward<T>(arg));
    processValues(std::forward<Ts>(args)...);
}
#endif //ENABLE_BASIC_FORWARDING_EXAMPLE

#if ENABLE_TUPLE_EXAMPLE

template<size_t idx, typename T>
struct GetHelper
{};

template<typename ...T>
struct Tuple
{};

template<typename T, typename ...Ts>
struct Tuple<T, Ts...>
{
    T first_element;
    Tuple<Ts...> remaining_elements;
    Tuple(const T& single_element, const Ts& ...elements)
    : first_element(single_element), remaining_elements(elements...)
    {
        cout<<"Tuple Constructor first element type = "<< boost::typeindex::type_id<T>().pretty_name()<< "\n";
        //cout<<"Tuple Constructor with DeclType = "<< boost::typeindex::type_id<decltype(single_element)>().pretty_name()<< "\n";
        //cout<<"Tuple Constructor with type_id_with_cvr = "<< boost::typeindex::type_id_with_cvr<decltype(single_element)>().pretty_name()<< "\n";
        cout<< __PRETTY_FUNCTION__ <<endl;
        cout<< "value single element: "<< single_element <<endl;
        size_t sizeof_remaining_elements = sizeof...(Ts);
        cout<< "size of remaining elements: "<< sizeof_remaining_elements<< endl;
    }
};


template<typename T, typename... Rest>
struct GetHelper<0, Tuple<T, Rest...>>
{ // Specialization for index 0
    static T get(const Tuple<T, Rest...> &data)
    {
        return data.first_element;
    }
};


template<size_t idx, typename T, typename... Rest>
struct GetHelper<idx, Tuple<T, Rest...>>
{ // GetHelper Implementation
    static auto get(const Tuple<T, Rest...> &data)
    {
        return GetHelper<idx - 1, Tuple<Rest...>>::get(data.remaining_elements);
    }
};


template<size_t idx, template <typename...> class Tuple, typename... Args>
auto get(const Tuple<Args...> &tuple_data)
{
    return GetHelper<idx, Tuple<Args...>>::get(tuple_data);
}

template<size_t Idx, typename... Args, typename Func>
auto tuple_at(const Tuple<Args...>& tp, const Func& func)
{
    const auto& v = ::get<Idx>(tp);
    func(v, Idx);
}

template<size_t Idx = 0, typename... Args , typename Func>
auto tuple_for(const Tuple<Args...>& tp, const Func& func)
{
    constexpr auto tuple_size = sizeof...(Args);
    if constexpr(Idx < tuple_size)
    {
        tuple_at<Idx>(tp, func);
        tuple_for<Idx + 1>(tp, func);
    }
}

#endif //ENABLE_TUPLE_EXAMPLE
/*TODO: use the get function to implement tuple_at function-> which basically takes polymorphic lambda and Tuple
 * data structure. Check if we can implement tuple_size_v */
int main()
{
    #if ENABLE_BASIC_FORWARDING_EXAMPLE
    int lvalue = 10;
    processValues(1, 3, 3.56, "test", 1.1f, lvalue);
    #endif
    #if ENABLE_TUPLE_EXAMPLE
    Tuple<bool> t1(false);
    Tuple<int, char, float> t2(1, 'a', 1.5);
    auto tuple_value_at = [=](const auto& v, int Idx){ cout<< "tuple value @ Index" << Idx << " = " << v << endl; };
    tuple_for(t2, tuple_value_at);
/*    tuple_at<1>(t2, tuple_value_at);
    cout<< "value in tuple using getter: " << get<0>(t2);*/
    #endif
    return 0;
}
