/**
 * 条款1：理解模板类别推导
 * 
*/

#include<iostream>
#include<typeinfo>

// template<typename T>
// void f(T& param);

// template<typename T>
// void f(const T& param);

// template<typename T>
// void f(T* param);

// template<typename T>
// void f(T&& param);

// template<typename T>
// void f(T param);

// template<typename T,std::size_t N>
// constexpr std::size_t arraySize(T (&)[N]) noexcept
// {
//     return N;
// }

template<typename T>
void f1(T param);

template<typename T>
void f2(T& param);

void someFunc(int,double);

int main()
{
    // int x = 27;  // x类别为int
    // const int cx = x;  // cx类别为const int
    // const int& rx = x;  // rx类别为const int&
    // const int *px = &x;

    // f(x);
    // f(cx);
    // f(rx);
    // f(27);
    // f(&x);
    // f(px);

    // int ketVals[] = {1,3,7,9};
    // int mappedVals[arraySize(ketVals)];  // mappedVals被指定与之相同
    // for(auto i : mappedVals)
    // {
    //     std::cout << i << " ";
    // }

    f1(someFunc);
    f2(someFunc);

    system("pause");
    return 0;
}