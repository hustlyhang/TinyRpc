# TinyRpc

简单的rpc实现，基于ZeroMQ  
首先实现序列化

序列化需要考虑传输的数据流字节序情况  
1． Little endian：将低序字节存储在起始地址  
2． Big endian：将高序字节存储在起始地址  

小端字节序比较符合人的思维，网络字节序采用Big endian，主机采用Little endian  

功能：  
	将字节流中的数据取出来以及放进去  