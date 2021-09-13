#include <string>
#include <iostream>
#include <ctime>
#include "TinyRpc.h"

#ifdef _WIN32
#include <Windows.h>  // use sleep
#else
#include <unistd.h>
#endif


#define buttont_assert(exp) { \
	if (!(exp)) {\
		std::cout << "ERROR: "; \
		std::cout << "function: " << __FUNCTION__  << ", line: " <<  __LINE__ << std::endl; \
		system("pause"); \
	}\
}\


struct PersonInfo
{
	int age;
	std::string name;
	float height;

	// must implement
	friend CSerializer& operator >> (CSerializer& in, PersonInfo& d) {
		in >> d.age >> d.name >> d.height;
		return in;
	}
	friend CSerializer& operator << (CSerializer& out, PersonInfo d) {
		out << d.age << d.name << d.height;
		return out;
	}
};

int main()
{
	CTinyRpc client;
	client.AsClient("127.0.0.1", 5555);
	client.SetTimeout(2000);

	int callcnt = 0;
	while (1) {
		std::cout << "current call count: " << ++callcnt << std::endl;

		client.Call<void>("foo_1");

		client.Call<void>("foo_2", 10);

		int foo3r = client.Call<int>("foo_3", 10).GetVal();
		buttont_assert(foo3r == 100);

		int foo4r = client.Call<int>("foo_4", 10, "TinyRpc", 100, (float)10.8).GetVal();
		buttont_assert(foo4r == 1000);

		PersonInfo  dd = { 10, "TinyRpc", 170 };
		dd = client.Call<PersonInfo>("foo_5", dd, 120).GetVal();
		buttont_assert(dd.age == 20);
		buttont_assert(dd.name == "TinyRpc is good");
		buttont_assert(dd.height == 180);

		// 调用类成员函数
		int foo6r = client.Call<int>("foo_6", 10, "buttonrpc", 100).GetVal();
		buttont_assert(foo6r == 1000);

		CTinyRpc::CValue_t<void> xx = client.Call<void>("foo_7", 666);
		buttont_assert(!xx.IsValid());

#ifdef _WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
	}

	return 0;
}
