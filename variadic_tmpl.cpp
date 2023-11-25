#include <iostream>
#include <vector>
#include <tuple>
#include <any>

//#include <typeinfo>
//#include <typeindex>
#include <initializer_list>
#include <boost/type_index.hpp>
#include <boost/type_traits.hpp>

#include <boost/function_types/function_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/function_arity.hpp>
#include <boost/typeof/std/utility.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/mp11.hpp>
#include <boost/callable_traits.hpp>

#ifdef ENABLE_BASIC_FORWARDING_EXAMPLE
#define ENABLE_BASIC_FORWARDING_EXAMPLE 1
#else
#define ENABLE_BASIC_FORWARDING_EXAMPLE 0
#endif

#ifdef ENABLE_TUPLE_EXAMPLE
#define ENABLE_TUPLE_EXAMPLE 1
#else 
#define ENABLE_TUPLE_EXAMPLE 0
#endif

using std::cout, std::endl, std::string;
using boost::function_traits;

using namespace boost::mp11;
using namespace boost::typeindex;
namespace ct = boost::callable_traits;

typedef struct
{
    int tag;
    int data;
}tagged_data;

typedef struct
{
    int tag;
    float data;
}tagged_data_float;

static auto sum {0};
void add (int a, int b, int c)
{
    sum = a+b+c;
}

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
        cout<<"From Tuple Constructor->first element type = "<< boost::typeindex::type_id<T>().pretty_name()<< "\n";
        //cout<<"Tuple Constructor with DeclType = "<< boost::typeindex::type_id<decltype(single_element)>().pretty_name()<< "\n";
        //cout<<"Tuple Constructor with type_id_with_cvr = "<< boost::typeindex::type_id_with_cvr<decltype(single_element)>().pretty_name()<< "\n";
        cout<< __PRETTY_FUNCTION__ <<endl;
        cout<< "From Tuple Constructor->value single element: "<< single_element <<endl;
        size_t sizeof_remaining_elements = sizeof...(Ts);
        cout<< "From Tuple Constructor->size of remaining elements: "<< sizeof_remaining_elements<< endl;
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
        tuple_at<Idx>(tp, func); // call the func on the passed tuple
        tuple_for<Idx + 1>(tp, func); //recursive call to tuple_for
    }
}

template<size_t Idx = 0, typename ...Args, typename Func>
auto tuple_any_of(const Tuple<Args...>& tp, const Func& func)
{
    constexpr auto tuple_size = sizeof...(Args);
    if constexpr (Idx < tuple_size)
    {
        bool result = func(::get<Idx>(tp));
        return result ? true: tuple_any_of<Idx + 1>(tp, func);
    }
    else
    {
        return false;
    }

}



#endif //ENABLE_TUPLE_EXAMPLE
/*TODO: Check if we can implement custom tuple_size_v  */
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

    auto has_f1_5 = tuple_any_of(t2, [](auto v){ return v == 1.5f; });
    has_f1_5 ? cout<< "value is found"<<endl : cout<< "tuple any_of can't find a value.."<<endl;
    const auto for_tuple_using_std = [](auto&& function, auto&& tup)
    {
        std::apply([&function](auto&& ...forward_tup)
            {
                (function(std::forward<decltype(forward_tup)>(forward_tup)),...); // NOTE: function folding expression expanding on parameter pack one at a time
            }, std::forward<decltype(tup)>(tup));
    };
    std::tuple t3 {2, 'b', 5.0};
    for_tuple_using_std([](const auto& x){cout<<"print using std apply: "<< x<< endl;}, t3);
    /*    tuple_at<1>(t2, tuple_value_at);
    cout<< "value in tuple using getter: " << get<0>(t2);*/
    #else
    std::cout<< "tuple demo is disabled"<<std::endl;
    #endif
//    std::vector<std::any> dt_records;
//    dt_records.emplace_back(tagged_data{});
//    dt_records.emplace_back(tagged_data_float{});
//    using traits = function_traits<decltype(add)> ;
//    std::cout<< traits::arity<<endl;
//    typedef BOOST_TYPEOF(add) add_func;
////    boost::mpl::at_c<boost::function_types::parameter_types<add_func>, 1>::type fnarg;
//    std::cout<<"add arg1 type : " << boost::typeindex::type_id<boost::mpl::at_c<boost::function_types::parameter_types<add_func>, 1>::type>().pretty_name()<<"\n";
//
//    using add_func_type = decltype(add);
//    std::cout <<"num of args in add_func_type : "<< std::tuple_size<ct::args_t<add_func_type>>::value << endl;
//    cout <<"add_func_type arg 0 : "<< type_id<mp_at_c<ct::args_t<add_func_type>,0>>().pretty_name() << endl;
//    cout <<"add_func_type arg 1 : "<< type_id<mp_at_c<ct::args_t<add_func_type>,1>>().pretty_name() << endl;
//    dt_records.emplace_back(traits::arg1_type{});
//    dt_records.emplace_back(mp_at_c<ct::args_t<add_func_type>,0>{});
//    for (auto& v: dt_records)
//    {
//        if(strcmp(v.type().name(), "11tagged_data") == 0 )
//        {
//            std::cout<<"type found: " << v.type().name()<<"\n";
//            auto val = std::any_cast<tagged_data>(v);
//            val.tag = 5;
//            val.data = 6;
//            std::cout<<val.tag  << "and "<< val.data<<"\n";
//        }
//        else
//        {
//            std::cout<<"type info : " << v.type().name()<<"\n";
//        }
//    }



    return 0;
}
