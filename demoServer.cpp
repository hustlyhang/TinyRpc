#include <string>
#include <iostream>
#include "TinyRpc.h"


#define buttont_assert(exp) { \
	if (!(exp)) {\
		std::cout << "ERROR: "; \
		std::cout << "function: " << __FUNCTION__  << ", line: " <<  __LINE__ << std::endl; \
		system("pause"); \
	}\
}\


// ²âÊÔÀý×Ó
void foo_1() {

}

void foo_2(int arg1) {
	buttont_assert(arg1 == 10);
}

int foo_3(int arg1) {
	buttont_assert(arg1 == 10);
	return arg1 * arg1;
}

int foo_4(int arg1, std::string arg2, int arg3, float arg4) {
	buttont_assert(arg1 == 10);
	buttont_assert(arg2 == "buttonrpc");
	buttont_assert(arg3 == 100);
	buttont_assert((arg4 > 10.0) && (arg4 < 11.0));
	return arg1 * arg3;
}

class ClassMem
{
public:
	int bar(int arg1, std::string arg2, int arg3) {
		buttont_assert(arg1 == 10);
		buttont_assert(arg2 == "buttonrpc");
		buttont_assert(arg3 == 100);
		return arg1 * arg3;
	}
};

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

PersonInfo foo_5(PersonInfo d, int weigth)
{
	buttont_assert(d.age == 10);
	buttont_assert(d.name == "buttonrpc");
	buttont_assert(d.height == 170);

	PersonInfo ret;
	ret.age = d.age + 10;
	ret.name = d.name + " is good";
	ret.height = d.height + 10;
	return ret;
}

int main()
{
	CTinyRpc server;
	server.AsServer(5555);

	server.Bind("foo_1", foo_1);
	server.Bind("foo_2", foo_2);
	server.Bind("foo_3", std::function<int(int)>(foo_3));
	server.Bind("foo_4", foo_4);
	server.Bind("foo_5", foo_5);

	ClassMem s;
	server.Bind("foo_6", &ClassMem::bar, &s);

	std::cout << "run rpc server on: " << 5555 << std::endl;
	server.Run();

	return 0;
}
