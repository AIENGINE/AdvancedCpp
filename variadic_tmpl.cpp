#include <iostream>
#include <vector>
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
    Tuple<Ts...> remaining_elemnts;
    Tuple(const T& single_element, const Ts& ...elements)
    : first_element(single_element), remaining_elemnts(elements...)
    {
        cout<<"Tuple Constructor first element = "<< boost::typeindex::type_id<T>().pretty_name()<< "\n";
        //cout<<"Tuple Constructor with DeclType = "<< boost::typeindex::type_id<decltype(single_element)>().pretty_name()<< "\n";
        //cout<<"Tuple Constructor with type_id_with_cvr = "<< boost::typeindex::type_id_with_cvr<decltype(single_element)>().pretty_name()<< "\n";
        cout<< __PRETTY_FUNCTION__ <<endl;
    }
};


#endif //ENABLE_TUPLE_EXAMPLE

int main()
{
    #if ENABLE_BASIC_FORWARDING_EXAMPLE
    int lvalue = 10;
    processValues(1, 3, 3.56, "test", 1.1f, lvalue);
    #endif
    #if ENABLE_TUPLE_EXAMPLE
    Tuple<bool> t1(false);
    Tuple<int, char, float> t2(1, 'a', 1.5);
    #endif
    return 0;
}
