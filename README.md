# TinyRpc

�򵥵�rpcʵ�֣�����ZeroMQ  
����ʵ�����л�

���л���Ҫ���Ǵ�����������ֽ������  
1�� Little endian���������ֽڴ洢����ʼ��ַ  
2�� Big endian���������ֽڴ洢����ʼ��ַ  

С���ֽ���ȽϷ����˵�˼ά�������ֽ������Big endian����������Little endian  

���ܣ�  
	���ֽ����е�����ȡ�����Լ��Ž�ȥ  

**Knowledge**:  
- tuple	Ԫ�飬��ģ�庯����Ҫ���� 1 ���䳤����ʱ��ʹ��tupleЧ�����ԡ�  
  - https://www.cnblogs.com/gudushishenme/archive/2013/09/22/3332582.html  
  - https://www.cnblogs.com/xueqiuqiu/articles/12955797.html  
- initializer_list
  -	https://blog.csdn.net/fengxinlinux/article/details/72614874
- std::index_sequence<I...>
  - https://blog.csdn.net/dboyguan/article/details/51706357
- �䳤����
  -	https://bygeek.cn/2018/09/28/variable-arguments-in-c-c/
- ���ù���
  -	https://bygeek.cn/2018/09/05/what-is-calling-convention/
- std::decay
  - �����Ƴ������е�һЩ���ԣ��������á�������volatile
- std::bind  
  - �������ɵ��ö����������һ����а󶨡��󶨺�Ľ������ʹ�� std::function���б��棬���ӳٵ��õ��κ�������Ҫ��ʱ��
  - ���ɵ��ö����������һ��󶨳�һ���º�����
  - ����Ԫ����������Ϊ n�� n>1���ɵ��ö���ת��һԪ���ߣ� n-1��Ԫ�ɵ��ö��󣬼�ֻ�󶨲��ֲ�����  
- std::enable_if
  - ֻ�е���һ��ģ�����Ϊ true ʱ��type ���ж��壬����ʹ�� type ������������	
  - `template <bool Cond, class T = void> struct enable_if;`