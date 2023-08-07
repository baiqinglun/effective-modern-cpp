# 条款1：理解模板类别推导

## auto用于模板类型推导

函数模板
```c++
template<typename T>
void f(ParamType param);
```
调用
```c++
f(expr);
```
> 在编译期间，通过expr推导T和ParamType类别，这两个类别往往不一样，ParamType常会包含一些修饰词，如const

若模板声明为
```c++
template<typename T>
void f(const T& param);
```
调用
```c++
int x = 0;
f(x);
```
此案例中，T被推导为int，ParamType被推导为const int&。但是T得推导结果不仅仅依赖于expr，还依赖于ParamType的形式，主要氛围3种。

### 形式一：ParamType是指针或引用，但不是万能引用

这样会导致
1. 若expr具有引用类别，现将引用部分忽略。
2. 尔后，对expr的列别和ParamType类别执行模式匹配，决定T的类别。

模板
```c++
template<typename T>
void f(T& param);
```
声明变量
```c++
int x = 27;  // x类别为int
const int cx = x;  // cx类别为const int
const int& rx = x;  // rx类别为const int&
```
调用
```c++
f(x);
f(cx);
f(rx);
```

编译代码
```c++
.LC0:
        .string "pause"
main:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     DWORD PTR [rbp-12], 27
        mov     eax, DWORD PTR [rbp-12]
        mov     DWORD PTR [rbp-16], eax
        lea     rax, [rbp-12]
        mov     QWORD PTR [rbp-8], rax
        lea     rax, [rbp-12]
        mov     rdi, rax
        call    void f<int>(int&)
        lea     rax, [rbp-16]
        mov     rdi, rax
        call    void f<int const>(int const&)
        mov     rax, QWORD PTR [rbp-8]
        mov     rdi, rax
        call    void f<int const>(int const&)
        mov     edi, OFFSET FLAT:.LC0
        call    system
        mov     eax, 0
        leave
        ret
```
可知
```c++
f(x);  // void f<int>(int&)。T为int，param为int&
f(cx);  // void f<int const>(int const&)。T为const int，param为const int&
f(rx);  // void f<int const>(int const&)。T为const int，param为const int&
```
- rx的引用性会在类别推导过程中被忽略。
> 向持有T&类别的模板传入const对象是安全的：该对象的常量性会成为T类别推导结果的组成部分。

**如果模板参数为const**
模板
```c++
template<typename T>
void f(const T& param);
```

编译结果
```c++
.LC0:
        .string "pause"
main:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     DWORD PTR [rbp-12], 27
        mov     eax, DWORD PTR [rbp-12]
        mov     DWORD PTR [rbp-16], eax
        lea     rax, [rbp-12]
        mov     QWORD PTR [rbp-8], rax
        lea     rax, [rbp-12]
        mov     rdi, rax
        call    void f<int>(int const&)
        lea     rax, [rbp-16]
        mov     rdi, rax
        call    void f<int>(int const&)
        mov     rax, QWORD PTR [rbp-8]
        mov     rdi, rax
        call    void f<int>(int const&)
        mov     edi, OFFSET FLAT:.LC0
        call    system
        mov     eax, 0
        leave
        ret
```
结果是
```c++
f(x);  // void f<int>(int const&)。T为int，param为const int&
f(cx);  // void f<int>(int const&)。T为int，param为const int&
f(rx);  // void f<int>(int const&)。T为int，param为const int&
```

**如果param是指针而非引用**
```c++
template<typename T>
void f(T* param);

int main()
{
    int x = 27;  // x类别为int
    const int *px = &x;  // 顶层const：不能修改px的值

    f(&x);
    f(px);

    system("pause");
    return 0;
}
```
编译结果
```c++
.LC0:
        .string "pause"
main:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     DWORD PTR [rbp-12], 27
        lea     rax, [rbp-12]
        mov     QWORD PTR [rbp-8], rax
        lea     rax, [rbp-12]
        mov     rdi, rax
        call    void f<int>(int*)
        mov     rax, QWORD PTR [rbp-8]
        mov     rdi, rax
        call    void f<int const>(int const*)
        mov     edi, OFFSET FLAT:.LC0
        call    system
        mov     eax, 0
        leave
        ret
```
结果
```c++
f(&x);  // void f<int>(int*)。T为int，param为int*
f(px);  // void f<int const>(int const*)。T为const int，param为const int*
```

### 形式二：ParamType是个万能引用

1. 如果expr是个左值，T和ParamType都会被推导为左值引用。
2. 如果expr是右值，应用形式一的规则。

```c++
template<typename T>
void f(T&& param);

int main()
{
    int x = 27;  // x类别为int
    const int cx = x;  // cx类别为const int
    const int& rx = x;  // rx类别为const int&

    f(x);  // void f<int&>(int&)
    f(cx);  // void f<int const&>(int const&)
    f(rx);  // void f<int const&>(int const&)
    f(27);  // void f<int>(int&&)

}
```

```c++
.LC0:
        .string "pause"
main:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 32
        mov     DWORD PTR [rbp-16], 27
        mov     eax, DWORD PTR [rbp-16]
        mov     DWORD PTR [rbp-20], eax
        lea     rax, [rbp-16]
        mov     QWORD PTR [rbp-8], rax
        lea     rax, [rbp-16]
        mov     rdi, rax
        call    void f<int&>(int&)
        lea     rax, [rbp-20]
        mov     rdi, rax
        call    void f<int const&>(int const&)
        mov     rax, QWORD PTR [rbp-8]
        mov     rdi, rax
        call    void f<int const&>(int const&)
        mov     DWORD PTR [rbp-12], 27
        lea     rax, [rbp-12]
        mov     rdi, rax
        call    void f<int>(int&&)
        mov     edi, OFFSET FLAT:.LC0
        call    system
        mov     eax, 0
        leave
        ret
```

**如果加const**
编译失败
```c++
f(x);  // 失败
f(cx);  // 失败
f(rx);  // 失败
f(27);  // 成功  void f<int>(int const&&)
```

### 形式三：ParamType既非指针又非引用

```c++
template<typename T>
void f(T param);
```
结果
```c++
int x = 27;  // x类别为int
const int cx = x;  // cx类别为const int
const int& rx = x;  // rx类别为const int

f(x);  // void f<int>(int)
f(cx);  // void f<int>(int)
f(rx);  // void f<int>(int)
```
- param是独立于cx和rx的对象（一个副本），cx和rx不能修改不能说明param不能修改。
- const和volatitle进会在按值传参时被忽略，如果形参是const的引用或指针，则expr的常量性会被保留。

编译结果
```c++
.LC0:
        .string "pause"
main:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 32
        mov     DWORD PTR [rbp-20], 27
        mov     eax, DWORD PTR [rbp-20]
        mov     DWORD PTR [rbp-4], eax
        lea     rax, [rbp-20]
        mov     QWORD PTR [rbp-16], rax
        mov     eax, DWORD PTR [rbp-20]
        mov     edi, eax
        call    void f<int>(int)
        mov     eax, DWORD PTR [rbp-4]
        mov     edi, eax
        call    void f<int>(int)
        mov     rax, QWORD PTR [rbp-16]
        mov     eax, DWORD PTR [rax]
        mov     edi, eax
        call    void f<int>(int)
        mov     edi, OFFSET FLAT:.LC0
        call    system
        mov     eax, 0
        leave
        ret
```
**如果形参为const**
```c++
const char* const ptr = "dadada";
f(ptr);  // void f<char const*>(char const*)。
```
- 星号左侧的const会被传递f，ptr涉及到的对象的常量性会被保留，自身的常量性会在传参时以复制的方式忽略。

## 数组实参

数组退化成首元素的指针
```c++
const char name[] = "J. p. Braiggs";
const char* ptrToName = name;
```

**按值传递模板**
```c++
template<typename T>
void f(T param);
```
结果
```c++
const char name[] = "J. p. Braiggs";
const char* ptrToName = name;
f(name);  // void f<char const*>(char const*)。推导为指针
```

因为指针退化，两者等价
```c++
void myF(int param[]);
void myF(int* param);
```

**引用法传递模板**
```c++
template<typename T>
void f(T& param);
```
结果
```c++
const char name[] = "J. p. Braiggs";
const char* ptrToName = name;
f(name);  // void f<char const [14]>(char const (&) [14])
```
**案例：获取数组个数**
```c++
template<typename T,std::size_t N>
constexpr std::size_t arraySize(T (&)[N]) noexcept
{
    return N;
}
int main()
{
    int ketVals[] = {1,3,7,9};
    int mappedVals[arraySize(ketVals)];  // mappedVals被指定与之相同
    for(auto i : mappedVals)
    {
        std::cout << i << " ";
    }
    // 输出为：0 0 0 0
}
```
- noexcept关键字告诉编译器函数不会发生异常，如果抛出错误，则调用std::terminate()函数，该函数内部会调用std::abort()终止程序。

## 函数实参
```c++
template<typename T>
void f1(T param);

template<typename T>
void f2(T& param);

template<typename T>
void f3(T* param);

void someFunc(int,double);
```
结果
```c++
f1(someFunc);  // void f1<void (*)(int, double)>(void (*)(int, double))
f2(someFunc);  // void f2<void (int, double)>(void (&)(int, double))
f3(someFunc);  // void f3<void (int, double)>(void (*)(int, double))
```