#include <iostream>
#include <vector>
#include <initializer_list>
#include <boost/type_index.hpp>

using std::cout, std::endl, std::string;

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
int main() {
    int lvalue = 10;
    processValues(1, 3, 3.56, "test", 1.1f, lvalue);
    return 0;
}
