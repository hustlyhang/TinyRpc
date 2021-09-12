# C++11 特性

- SFINA
  - 匹配失败不是错误，当调用模板函数时编译器会根据传入参数推导最合适的模板函数，在这个推导过程中如果某一个或者某几个模板函数推导出来是编译无法通过的，只要有一个可以正确推导出来，那么那几个推导得到的可能产生编译错误的模板函数并不会引发编译错误。
  - ```C++
    struct Test {
        typedef int foo;
    };

    template <typename T> 
    void f(typename T::foo) {} // Definition #1

    template <typename T> 
    void f(T) {}               // Definition #2

    int main() {
        f<Test>(10); // Call #1.
        f<int>(10);  // Call #2. Without error (even though there is no int::foo) thanks to SFINAE.
    }
    ```
    在上面代码中即使int::foo是一个错误的语法，也不会报错
- [std::enable_if](https://www.jianshu.com/p/a961c35910d2)
  - ```C++
    // 普通模板定义
    template<bool B, class T = void>
    struct enable_if {};
    // 偏特化模板定义，当第一个模板参数为false的时候并不会定义type，只有第一个模板参数为true的时候才会定义type
    template<class T>
    struct enable_if<true, T> { typedef T type; };
    ``` 
    核心就是应用SFINAE原则，来实现调用函数去推导正确的模板函数版本。
    
    
- [std::forward](https://www.jianshu.com/p/b90d1091a4ff)
  - 引用折叠：如果间接的创建一个引用的引用，则这些引用就会“折叠”。在所有情况下（除了一个例外），引用折叠成一个普通的左值引用类型。一种特殊情况下，引用会折叠成右值引用，即右值引用的右值引用， T&& &&。
      - ```X& &、X& &&、X&& &都折叠成X&```
      - ```X&& &&折叠为X&&```

  - 右值引用的特殊类型推断规则：当将一个左值传递给一个参数是右值引用的函数，且此右值引用指向模板类型参数(T&&)时，编译器推断模板参数类型为实参的左值引用，如
    ```C++
    template<typename T> 
    void f(T&&);

    int i = 42;
    f(i)
    ```
    此时f(i)中传入的就不是int i了，而是int& i;  
    根据这两个规则，可以得知：如果一个函数形参是一个指向模板类型的右值引用，则该参数可以绑定到一个左值上。
  - static_cast：虽然不能隐式的将一个左值转换为右值引用，但是可以通过static_cast显示地将一个左值转换为一个右值。（C++ 11）
  - 完美转发：
      > 完美转发实现了参数在传递过程中保持其值属性的功能，即若是左值，则传递之后仍然是左值，若是右值，则传递之后仍然是右值。
    ```C++
    class Foo
    {
    public:
        std::string member;

        template<typename T>
        Foo(T&& member): member{std::forward<T>(member)} {}
    };
    ```
    这个例子中，如果传入的参数为lvalue,那么根据上面的**右值引用的特殊类型推断规则**，可知T被推断为```std::string& ```类型，那么就变为 ```std::string& &&```
    根据**引用折叠**那么就相当于```std::string&```类型；  
    传入的如果是一个右值，rlvalue， 模板推导之后 T 为 ```std::string``` 那么就变为```std::string&&``` 也就是右值引用，最后会调用右值构造函数；
  - std::move()和std::forward()对比
    > - std::move执行到右值的无条件转换。就其本身而言，它没有move任何东西。
    > - std::forward只有在它的参数绑定到一个右值上的时候，它才转换它的参数到一个右值。
    > - std::move和std::forward只不过就是执行类型转换的两个函数；std::move没有move任何东西，std::forward没有转发任何东西。在运行期，它们没有做任何事情。它们没有产生需要执行的代码，一byte都没有。
    > - std::forward<T>()不仅可以保持左值或者右值不变，同时还可以保持const、Lreference、Rreference、validate等属性不变；

- std::move
  - ```C++
    template<typename T>
    typename remove_reference<T>::type && move(T&& t)
    {
        return static_cast<typename remove_reference<T>::type &&>(t);
    }
    ```
    可知接受的是一个右值引用类型，根据引用折叠规则，所以可以接受任意类型的实参。move自身除了做一些参数的判断之外，返回右值引用本质上还是靠static_cast<T&&>完成
  
  
- std::decay
  - 移除类型中的一些特性，比如引用、常量、volatile，但是注意不包括指针特性，这个类就是std::decay，在头文件type_traits（类型萃取）中
  - ```C++
    template<class _Ty>
	  struct decay
	  {	// determines decayed version of _Ty
	  using _Ty1 = remove_reference_t<_Ty>;   // 移除引用

	  using type = conditional_t<is_array_v<_Ty1>,  // 如果是数组的话
		  add_pointer_t<remove_extent_t<_Ty1>>,       // 取出数组的原始类型
		  conditional_t<is_function_v<_Ty1>,          // 如果是函数
			  add_pointer_t<_Ty1>,                      // 变为函数指针
			  remove_cv_t<_Ty1>>>;                      // 不是函数，移除CV特性（const, volatile）  
	  };

    template<class _Ty>
	    using decay_t = typename decay<_Ty>::type;
    ```
    经典样例
    ```C++
    template <typename T, typename U>
    struct decay_equiv : 
    std::is_same<typename std::decay<T>::type, U>::type 
    {};

    int main()
    {
        std::cout << std::boolalpha
                  << decay_equiv<int, int>::value << '\n'
                  << decay_equiv<int&, int>::value << '\n'
                  << decay_equiv<int&&, int>::value << '\n'
                  << decay_equiv<const int&, int>::value << '\n'
                  << decay_equiv<int[2], int*>::value << '\n'
                  << decay_equiv<int(int), int(*)(int)>::value << '\n';
    }
    // 全是true
    ```
  
