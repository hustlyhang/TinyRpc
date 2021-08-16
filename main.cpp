
#include "TinyRpc.h"

void foo(int arg1) {
	printf("foo :%d\n", arg1);
}


int main() {
	CTinyRpc RpcServer;
	RpcServer.AsServer(9999);
	RpcServer.Bind("foo", foo);
	RpcServer.Run();
	return 0;
}