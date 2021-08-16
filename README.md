# TinyRpc

简单的rpc实现，基于ZeroMQ  
首先实现序列化

序列化需要考虑传输的数据流字节序情况  
1． Little endian：将低序字节存储在起始地址  
2． Big endian：将高序字节存储在起始地址  

小端字节序比较符合人的思维，网络字节序采用Big endian，主机采用Little endian  

功能：  
	将字节流中的数据取出来以及放进去  

**Knowledge**:  
- tuple	元组，当模板函数需要超过 1 个变长参数时，使用tuple效果明显。  
  - https://www.cnblogs.com/gudushishenme/archive/2013/09/22/3332582.html  
  - https://www.cnblogs.com/xueqiuqiu/articles/12955797.html  
- initializer_list
  -	https://blog.csdn.net/fengxinlinux/article/details/72614874
- std::index_sequence<I...>
  - https://blog.csdn.net/dboyguan/article/details/51706357
- 变长参数
  -	https://bygeek.cn/2018/09/28/variable-arguments-in-c-c/
- 调用惯例
  -	https://bygeek.cn/2018/09/05/what-is-calling-convention/
- std::decay
  - 用以移除类型中的一些特性，比如引用、常量、volatile
- std::bind  
  - 用来将可调用对象与其参数一起进行绑定。绑定后的结果可以使用 std::function进行保存，并延迟调用到任何我们需要的时候。
  - 将可调用对象与其参数一起绑定成一个仿函数。
  - 将多元（参数个数为 n， n>1）可调用对象转成一元或者（ n-1）元可调用对象，即只绑定部分参数。  
- std::enable_if
  - 只有当第一个模板参数为 true 时，type 才有定义，否则使用 type 会产生编译错误。	
  - `template <bool Cond, class T = void> struct enable_if;`