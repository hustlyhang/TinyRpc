# TinyRpc

- 简单的rpc实现，基于ZeroMQ  
  - 简单易用
  - 服务端目前可以绑定自由函数、类成员函数等
  - 服务端函数参数可以是任意类型任意数量

- 文件
  - 主要实现在Serializer.h和TinyRpc.h两个文件中
  - demo中提供了一些例子。

- 流程
  ![flow](https://github.com/hustlyhang/TinyRpc/blob/master/src/rpc.png)
