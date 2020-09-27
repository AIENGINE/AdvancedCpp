#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <cmath>
#include <boost/type_index.hpp>
#include <cctype>
#include <type_traits>

using namespace std;
namespace Collection
{
    /*Higher Order functions: Abstract away problems into Range, Pred, Actions that will be input from Client code*/

    template<typename ContainerType>
    auto log_output(ostream& os, const ContainerType& container, const string& delimeter)
    {
        if constexpr (!is_arithmetic_v<ContainerType>)
        copy(begin(container), end(container), std::ostream_iterator<typename ContainerType::value_type>
                (os, delimeter.c_str()));
        else
            copy(begin(container), end(container), std::ostream_iterator<ContainerType>
                    (os, delimeter.c_str()));

/*        cout<<"Type of ContainerType = "
        << boost::typeindex::type_id<typename ContainerType::value_type>().pretty_name()<< "\n";*/
    }

    template<typename FunctionType, typename RangeType>
    RangeType mapfunction(FunctionType&& f, RangeType r)
    {
        transform(begin(r), end(r), begin(r), forward<FunctionType>(f));
        return r;
    }


    template<typename FunctionType, typename KeyType, typename ValueType>
    map<KeyType, ValueType> mapfunction(FunctionType&& f, const map<KeyType, ValueType>& map_container)
    {
        map<KeyType, ValueType> modified_map;
        for(const auto kvp: map_container)
        {
            modified_map.insert(f(kvp));
        }

        return modified_map;
    }

    template<typename FunctionType, typename RangeType, typename T>
    constexpr T fold_left_func(FunctionType&& f, RangeType&& r, T init_value)
    {
        return accumulate(begin(r), end(r), move(init_value), forward<FunctionType>(f));
    }

    template<typename FunctionType, typename RangeType, typename T>
    constexpr T fold_right_func(FunctionType&& f, RangeType&& r, T init_value)
    {
        return accumulate(rbegin(r), rend(r), move(init_value), forward<FunctionType>(f));
    }

    template<typename FunctionType1, typename FunctionType2>
    auto compose_function(FunctionType1&& f, FunctionType2&& g)
    {
        return [=](auto x) { return f(g(x)); };
    }

    template<typename FunctionType1, typename ...FunctionType2>
    auto compose_function(FunctionType1&& f, FunctionType2&&...g)
    {
        return [=](auto x){ return f(compose_function(g...)(x)); };
    }


}

/*templatized functor which acts as specific version of the action/function based on range/data passed*/
template<typename T=double >
struct roundfunc
{
    typename enable_if<is_floating_point<T>::value, T>::type operator()(const T& value)
    {
        return round(value);
    }

};



int main(int argc, char** argv)
{
    auto vec_nums = vector<int>{0, 2, -3, 5, -1, 6, 8, -4, 9};
    auto amounts = array<double, 5>{10.42, 2.50, 100.0, 23.75, 12.99};
    auto words = map<string, int>{{"one", 1}, {"two", 2}, {"three", 3}};
    //function abs: applied to every element in the container in case of vector
    auto result = Collection::mapfunction([](const int i){ return abs(i); }, vec_nums);
    Collection::log_output(cout, result, ",");
    cout<<endl;
    auto result_round = Collection::mapfunction(roundfunc<>(), amounts);
    Collection::log_output(cout, result_round, " ");
    cout<<endl;
    auto result_map = Collection::mapfunction([](pair<string, int> const kvp)
        {
            return make_pair(Collection::mapfunction([](unsigned char c){ return toupper(c); }, kvp.first), kvp.second);

        }, words);
    auto map_iter = begin(result_map);
    cout<< map_iter->first << " "<<(++map_iter)->first<< " "<< (++map_iter)->first;
    cout<<endl;
    Collection::log_output(cout, map_iter->first, "");
    cout<<endl;
    auto sum = Collection::fold_left_func(plus<>(), vec_nums, 0);
    cout<<sum<<endl;
    auto pipeline_sum = Collection::fold_right_func(plus<>(), Collection::mapfunction([](const int i){return i*i;},
            Collection::mapfunction([](const int j){return abs(j); }, vec_nums)),0);
    cout<<pipeline_sum<<endl;

    auto v = Collection::compose_function(
            [](int n) {return std::to_string(n); },
            [](int n) {return n * n; },
            [](int n) {return n + n; },
            [](int n) {return std::abs(n); })(-3);
    cout<<"Compose pipeline sum: "<<v<<endl;

    return 0;
}