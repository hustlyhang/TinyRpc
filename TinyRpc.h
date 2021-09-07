#pragma once
#include "demo/depends/include/zmq.h"
#include "Serializer.h"
#include <map>
#include <functional>
#include <sstream>
#include <memory>

class CSerializer;

template <typename T>
struct SType_xx {
	typedef T type;
};

template<>
struct SType_xx<void> {
	typedef int8_t type;
};

// 调用时对参数进行打包
template <typename... Args>
void PackageParams(CSerializer& _s, const std::tuple<Args...>& _t) {
	_PackageParams(_s, _t, std::index_sequence_for<Args...>{});
}

template <typename Tuple, std::size_t... Is>
void _PackageParams(CSerializer& _s, const Tuple& _t, std::index_sequence<Is...>) {
	initializer_list<int>{((_s << std::get<Is>(_t)), 0)...};
}

// 用tuple做参数调用函数模板类
template<typename Function, typename Tuple, std::size_t... Index>
decltype(auto) _Invoke(Function&& func, Tuple&& t, std::index_sequence<Index...>)
{
	return func(std::get<Index>(std::forward<Tuple>(t))...);
}

template<typename Function, typename Tuple>
decltype(auto) Invoke(Function&& func, Tuple&& t)
{
	constexpr auto size = std::tuple_size<typename std::decay<Tuple>::type>::value;
	return _Invoke(std::forward<Function>(func), std::forward<Tuple>(t), std::make_index_sequence<size>{});
}

// 调用帮助类，主要用于返回是否void的情况
template<typename R, typename F, typename ArgsTuple>
typename std::enable_if<std::is_same<R, void>::value, typename SType_xx<R>::type >::type
CallHelper(F f, ArgsTuple args) {
	Invoke(f, args);
	return 0;
}

template<typename R, typename F, typename ArgsTuple>
typename std::enable_if<!std::is_same<R, void>::value, typename SType_xx<R>::type >::type
CallHelper(F f, ArgsTuple args) {
	return Invoke(f, args);
}

class CTinyRpc {
public:
	CTinyRpc();
	~CTinyRpc();
public:
	// 远程调用后记录调用函数相关信息
	template <class T>
	class CValue_t {
	public:
		typedef typename SType_xx<T>::type type;

		CValue_t() { 
			m_iCode = 0;
			m_strMsg.clear();
		}
		bool IsValid() {
			return m_iCode == 0;
		}
		int GetErrorCode() {
			return m_iCode;
		}
		std::string GetErrorMsg() {
			return m_strMsg;
		}
		type GetVal() {
			return m_sVal;
		}
		void SetErrorCode(int _code) {
			m_iCode = _code;
		}
		void SetErrorMsg(std::string _msg) {
			m_strMsg = _msg;
		}
		void SetVal(const type& _val) {
			m_sVal = _val;
		}
		friend CSerializer& operator >> (CSerializer& _in, CValue_t<T>& _d) {
			_in >> _d.m_iCode >> _d.m_strMsg;
			if (_d.m_iCode == 0) _in >> _d.m_sVal;
			return _in;
		}
		friend CSerializer& operator << (CSerializer& _out, CValue_t<T>& _d) {
			_out << _d.m_iCode << _d.m_strMsg << _d.m_sVal;
			return _out;
		}

	private:
		int m_iCode;
		std::string m_strMsg;
		type m_sVal;
	};
public:
	// client
	void AsClient(std::string _ip, int _port);
	// server
	void AsServer(int _port);
	// 收发数据
	void Recv(zmq::message_t& _data);
	void Send(zmq::message_t& _data);
	// 超时
	void SetTimeout(uint32_t _ms);
	// 绑定服务端函数接口
	template <typename F>
	void Bind(std::string _funcName, F _func);
	template <typename F, typename S>
	void Bind(std::string _funcName, F _func, S* _s);


	void Run();
public:
	enum class RPCROLE {
		CLIENT,
		SERVER
	};
	enum class RPCERRCODE {
		SUCCESS,
		TIMEOUT,
		NOTBIND
	};

public:
	// 客户端调用函数
	template <typename R, typename... Params>
	CValue_t<R> Call(std::string _name, Params... _ps) {
		using args_type = std::tuple<typename std::decay<Params>::type...>;
		args_type args = std::make_tuple(_ps);
		// 将函数名和参数序列化后传给远端调用
		CSerializer s;
		s << _name;
		// 将参数打包
		PackageParams(s, args);
		return NetCall<R>(s);
	}
	template <typename R>
	CValue_t<R> Call(std::string _name) {
		CSerializer ds;
		ds << _name;
		return NetCall<R>(ds);
	}

private:
	template<typename R>
	CValue_t<R> NetCall(CSerializer& ds);

	CSerializer* _Call(std::string _name, const char* _data, int _len);

	template<typename F>
	void Callproxy(F fun, CSerializer* pr, const char* data, int len) {
		_Callproxy(fun, pr, data, len);
	}

	template<typename F, typename S>
	void Callproxy(F _fun, S* _s, CSerializer* _pr, const char* _data, int _len) {
		_Callproxy(_fun, _s, _pr, _data, _len);
	}


	template<typename R, typename... Params>
	void _Callproxy(R(*func)(Params...), CSerializer* pr, const char* data, int len) {
		_Callproxy(std::function<R(Params...)>(func), pr, data, len);
	}

	template<typename R, typename... Params>
	void _Callproxy(std::function<R(Params... ps)> func, CSerializer* pr, const char* data, int len);

	template<typename R, typename C, typename S, typename... Params>
	void _Callproxy(R(C::*func)(Params... ps), S* s, CSerializer* pr, const char* data, int len);



private:
	zmq::context_t m_cContext;
	RPCERRCODE m_eErrCode;
	RPCROLE m_eRole;
	// 存放函数名和函数的映射
	std::map<std::string, std::function<void(CSerializer*, const char*, int)>> m_mHandler;
	std::unique_ptr<zmq::socket_t, std::function<void(zmq::socket_t*)>> m_pSocket;
};

inline CTinyRpc::CTinyRpc() :m_cContext(1) {
	m_eRole = RPCROLE::CLIENT;
	m_eErrCode = RPCERRCODE::SUCCESS;
}

inline CTinyRpc::~CTinyRpc() {
	m_cContext.close();
}

inline void CTinyRpc::AsClient(std::string _ip, int _port) {
	m_eRole = RPCROLE::CLIENT;
	auto deleterLambda = [](zmq::socket_t* sock) {
		sock->close();
		delete sock;
		sock = nullptr;
	};
	m_pSocket = std::unique_ptr<zmq::socket_t, decltype(deleterLambda)>(new zmq::socket_t(m_cContext, ZMQ_REQ), deleterLambda);
	ostringstream os;
	os << "tcp://" << _ip << ":" << _port;
	m_pSocket->connect (os.str());
}

inline void CTinyRpc::AsServer(int _port) {
	m_eRole = RPCROLE::SERVER;
	auto deleterLambda = [](zmq::socket_t* sock) {
		sock->close();
		delete sock;
		sock = nullptr;
	};
	m_pSocket = std::unique_ptr<zmq::socket_t, decltype(deleterLambda)>(new zmq::socket_t(m_cContext, ZMQ_REP), deleterLambda);
	ostringstream os;
	os << "tcp://*:" << _port;
	m_pSocket->bind (os.str());
}

inline void CTinyRpc::Send(zmq::message_t& _data) {
	m_pSocket->send(_data);
}

inline void CTinyRpc::Recv(zmq::message_t& _data) {
	m_pSocket->recv(&_data);
}

inline void CTinyRpc::SetTimeout(uint32_t _ms) {
	if (m_eRole == RPCROLE::CLIENT) m_pSocket->setsockopt(ZMQ_RCVTIMEO, _ms);
}

// server
template <typename F>
inline void CTinyRpc::Bind(std::string _funcName, F _func) {
	m_mHandler[_funcName] = std::bind(&CTinyRpc::Callproxy<F>, this, _func, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

template <typename F, typename S>
inline void CTinyRpc::Bind(std::string _funcName, F _func, S* _s) {
	m_mHandler[_funcName] = std::bind(&CTinyRpc::Callproxy<F, S>, this, _func, _s, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}


// 函数指针
template<typename R, typename... Params>
void CTinyRpc::_Callproxy(std::function<R(Params... ps)> func, CSerializer* pr, const char* data, int len) {

	using args_type = std::tuple<typename std::decay<Params>::type...>;

	CSerializer ds(CStreamBuf(data, len));
	constexpr auto N = std::tuple_size<typename std::decay<args_type>::type>::value;
	args_type args = ds.GetTuple < args_type >(std::make_index_sequence<N>{});

	typename SType_xx<R>::type r = CallHelper<R>(func, args);

	CValue_t<R> val;
	val.SetErrorCode((int)RPCERRCODE::SUCCESS);
	val.SetVal(r);
	(*pr) << val;
}

// 类成员函数
template<typename R, typename C, typename S, typename... Params>
void CTinyRpc::_Callproxy(R(C::*func)(Params... ps), S* s, CSerializer* pr, const char* data, int len) {
	using args_type = std::tuple<typename std::decay<Params>::type...>;

	CSerializer ds(CStreamBuf(data, len));

	constexpr auto N = std::tuple_size<typename std::decay<args_type>::type>::value;

	args_type args = ds.GetTuple<args_type>(std::make_index_sequence<N>{});

	auto ff = [=](Params..ps) {
		return(s->*func)(ps...);
	};

	// 告诉编译器后面跟着的是一个类型而不是变量
	typename SType_xx<R>::type r = CallHelper<R>(ff, args);

	CValue_t<R> val;
	val.SetErrorCode((int)RPCERRCODE::SUCCESS);
	val.SetVal(r);
	(*pr) << val;
}

// 不断地接收数据，然后转换成初始化CSerializer类
inline void CTinyRpc::Run() {
	if (m_eRole != RPCROLE::SERVER) return;
	while (1) {
		zmq::message_t data;
		Recv(data);
		CStreamBuf iodev((char*)data.data(), data.size());
		CSerializer ds(iodev);
		std::string funname;
		ds >> funname;
		CSerializer* r = _Call(funname, ds.GetCurData(), ds.Size() - funname.size());
		zmq::message_t retmsg(r->Size());
		memcpy(retmsg.data(), r->GetData(), r->Size());
		Send(retmsg);
		delete r;
	}
}

inline CSerializer* CTinyRpc::_Call(std::string _name, const char* _data, int _len) {
	CSerializer* ds = new CSerializer();
	if (m_mHandler.find(_name) == m_mHandler.end()) {
		(*ds) << (int)RPCERRCODE::NOTBIND;
		(*ds) << ("function not bind: " + _name);
		return ds;
	}
	auto fun = m_mHandler[_name];
	fun(ds, _data, _len);
	ds->Reset();
	return ds;
}

template <typename R>
inline CTinyRpc::CValue_t<R> CTinyRpc::NetCall(CSerializer& _s) {
	zmq::message_t request(_s.Size() + 1);
	memcpy(request.data(), _s.GetData(), _s.Size());
	if (m_eErrCode != RPCERRCODE::TIMEOUT) Send(request);
	zmq::message_t reply;
	Recv(reply);
	CValue_t<R> val;
	if (reply.size() == 0) {
		// timeout
		m_eErrCode = RPCERRCODE::TIMEOUT;
		val.SetErrorCode((int)RPCERRCODE::TIMEOUT);
		val.SetErrorMsg("recv timeout");
		return val;
	}
	m_eErrCode = RPCERRCODE::SUCCESS;
	_s.Clear();
	_s.WriteRawData((char*)reply.data(), reply.size());
	_s.Reset();
	_s >> val;
	return val;
}