#pragma once
#include "./include/zmq.hpp"
#include "Serializer.h"

class CSerializer;

template <typename T>
struct sType_xx {
	typedef T type;
};

template<>
struct sType_xx<void> {
	typedef int8_t type;
};

// 调用时对参数进行打包
template <typename... Args>
void PackageParams(CSerializer& _s, const std::tuple<Args...>& _t) {
	PackageParams_t(_s, _t, std::index_sequence_for<Args...>{})
}

template <typename Tuple, std::size_t... Is>
void PackageParams_t(CSerializer& _s, const Tuple& _t, std::index_sequence<Is...>) {
	initializer_list<int>{((_s << std::get<Is>(_t)), 0)...};
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
		typedef template sType_xx<T>::type type;
		typedef std::string msg_type;
		typedef uint16_t code_type;

		CValue_t() { 
			m_uCode = 0;
			m_strMsg.clear();
		}
		bool IsValid() {
			return m_uCode == 0;
		}
		code_type GetErrorCode() {
			return m_uCode;
		}
		msg_type GetErrorMsg() {
			return m_strMsg;
		}
		type GetVal() {
			return m_sVal;
		}
		void SetErrorCode(code_type _code) {
			m_uCode = _code;
		}
		void SetErrorMsg(msg_type _msg) {
			m_strMsg = _msg;
		}
		void SetVal(const type& _val) {
			m_sVal = _val;
		}
		friend CSerializer& operator >> (CSerializer& _in, CValue_t<T>& _d) {
			_in >> _d.m_uCode >> _d.m_strMsg;
			if (_d.m_uCode == 0) _in >> _d.m_sVal;
			return _in;
		}
		friend CSerializer& operator << (CSerializer& _out, CValue_t<T>& _d) {
			_out << _d.m_uCode << _d.m_strMsg << _d.m_sVal;
			return _out;
		}

	private:
		code_type m_uCode;
		msg_type m_strMsg;
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

	void Run();
public:
	enum class RPCROLE {
		CLIENT,
		SERVER
	};
	enum class RPCERRCODE {
		SUCCESS,
		TIMEOUT
	};

public:
	// 客户端调用函数
	template <typename R, typename... Params>
	CValue_t<R> Call(std::string _name, Params... _ps) {
		using args_type = std::tuple<typename std::decay<Params>::type...>;
		args_type args = std::make_tuple(_ps);
		// 将函数名和参数序列化后传给远端
		CSerializer s;
		s << _name;
		PackageParams(s, args);
		return NetCall<R>(s);
	}


private:
	template<typename R>
	CValue_t<R> NetCall(CSerializer& ds);

private:
	zmq::context_t m_cContext;
	RPCERRCODE m_eErrCode;
	RPCROLE m_eRole;
};


inline CTinyRpc::CTinyRpc() :m_cContext(1) {
	m_eErrCode = RPCERRCODE::SUCCESS;
}

inline CTinyRpc::~CTinyRpc() {
	m_cContext.close();
}

inline void CTinyRpc::AsClient(std::string _ip, int _port) {
	// TODO
}

inline void CTinyRpc::AsServer(int _port) {
	// TODO
}

inline void CTinyRpc::Send(zmq::message_t& _data) {
	// TODO
}

inline void CTinyRpc::Recv(zmq::message_t& _data) {
	// TODO
}

inline void CTinyRpc::Run() {
	// TODO
}


template <typename R>
inline CTinyRpc::CValue_t<R> CTinyRpc::NetCall(CSerializer& _s) {
	//TODO
}