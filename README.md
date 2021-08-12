# TinyRpc

简单的rpc实现，基于ZeroMQ
首先实现序列化

序列化需要考虑传输的数据流字节序情况
1． Little endian：将低序字节存储在起始地址
2． Big endian：将高序字节存储在起始地址

小端字节序比较符合人的思维，网络字节序采用Big endian，主机采用Little endian

功能：
	将字节流中的数据取出来以及放进去

Knowledge:
	一、tuple	元组，当模板函数需要超过 1 个变长参数时，使用tuple效果明显。
		https://www.cnblogs.com/gudushishenme/archive/2013/09/22/3332582.html
		https://www.cnblogs.com/xueqiuqiu/articles/12955797.html
	二、initializer_list
		https://blog.csdn.net/fengxinlinux/article/details/72614874
	三、std::index_sequence<I...>
		https://blog.csdn.net/dboyguan/article/details/51706357
	四、变长参数
		https://bygeek.cn/2018/09/28/variable-arguments-in-c-c/
	五、调用惯例
		https://bygeek.cn/2018/09/05/what-is-calling-convention/
	六、std::decay
		用以移除类型中的一些特性，比如引用、常量、volatile
		