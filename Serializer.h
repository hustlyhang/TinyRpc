#pragma once

#include <vector>
#include <sstream>
#include <algorithm>
#include <cstdint>
#include <utility>
#include <tuple>
/// #include <tuple>

using namespace std;

// 利用vector来实现字节流处理

class CStreamBuf : public vector<char> {
public:
	CStreamBuf() {
		// 初始化
		m_iPos = 0;
	}
	CStreamBuf(const char* _in, size_t _len) {
		m_iPos = 0;
		insert(begin(), _in, _in + _len);
	}
	~CStreamBuf(){}
	// 重置
	void Reset() {
		m_iPos = 0;
	}

	// 获取所有数据
	const char* GetData() {
		return &(*this)[0];
	}

	// 获取当前数据
	const char* GetCurData() {
		return &(*this)[m_iPos];
	}

	// 偏移
	void Offset(int _k) {
		m_iPos += _k;
	}

	// 是否结束
	bool IsEOF() {
		return m_iPos >= (int)size();
	}

	// 输入
	void Input(char* _in, size_t _len) {
		insert(end(), _in, _in + _len);
	}

	// 查找
	int FindChar(char _c) {
		iterator itr = find(begin() + m_iPos, end(), _c);
		if (itr != end())
		{
			return itr - (begin() + m_iPos);
		}
		return -1;
	}
private:
	int m_iPos;		// 记录当前字节流的位置
};


// 序列化类
class CSerializer {
public:
	// 默认字节序为小端字节序
	CSerializer() { m_eByteOrder = ByteOrder::Little; }
	~CSerializer() { }
	CSerializer(CStreamBuf _dev, int _byteOrder = 1) {
		m_eByteOrder = (ByteOrder)_byteOrder;
		m_cIoDevice = _dev;
	}
public:
	// 字节序，大端小端
	enum class ByteOrder {
		Big,
		Little
	};
public:
	void Reset() {
		m_cIoDevice.Reset();
	}
	
	int Size() {
		return m_cIoDevice.size();
	}

	void Clear() {
		m_cIoDevice.clear();
		Reset();
	}

	const char* GetData() {
		return m_cIoDevice.GetData();
	}

	const char* GetCurData() {
		return m_cIoDevice.GetCurData();
	}
	// 转换字节序
	void ChangeByteOrder(char* _in, int _len) {
		if (m_eByteOrder == ByteOrder::Big) {
			reverse(_in, _in + _len);
		}
	}
	// 添加数据
	void WriteRawData(char* _in, int _len) {
		m_cIoDevice.Input(_in, _len);
		m_cIoDevice.Offset(_len);
	}

public:
	// 将buffer里面的内容输出到tuple中去
	template<typename Tuple, std::size_t Id>
	void GetV(CSerializer& _ds, Tuple& t) {
		_ds >> std::get<Id>(t);
	}

	template<typename Tuple, std::size_t... Is>
	Tuple GetTuple(std::index_sequence<Is...>) {
		Tuple t;
		initializer_list<int>{((GetV<Tuple, Is>(*this, t)), 0)...};
		return t;
	}

	template<typename T>
	void OutputType(T& t);

	template<typename T>
	void InputType(T t);

	// 重载operator <<、>>
	template <typename T>
	CSerializer& operator<< (T t) {
		InputType(t);
		return *this;
	}
	template <typename T>
	CSerializer& operator>> (T& t) {
		OutputType(t);
		return *this;
	}
private:
	ByteOrder m_eByteOrder;
	CStreamBuf m_cIoDevice;
};

// 将streambuffer中的数据提取出来
template <typename T>
inline void CSerializer::OutputType(T& _t) {
	int tSize = sizeof T;
	char* data = new char[tSize];
	// 如果缓冲流中还有数据，就输出数据
	if (!m_cIoDevice.IsEOF()) {
		memcpy(data, m_cIoDevice.GetCurData(), tSize);
		m_cIoDevice.Offset(tSize);
		// 转换字节序
		ChangeByteOrder(data, tSize);
		// 强制类型转换
		_t = *reinterpret_cast<T*>(&data[0]);
	}
	delete[] data;
}
 // 特化
template<>
inline void CSerializer::OutputType(std::string& _in) {
	int marklen = sizeof(uint16_t);
	char* d = new char[marklen];
	memcpy(d, m_cIoDevice.GetCurData(), marklen);
	ChangeByteOrder(d, marklen);
	int len = *reinterpret_cast<uint16_t*>(&d[0]);
	m_cIoDevice.Offset(marklen);
	delete[] d;
	if (len == 0) return;
	_in.insert(_in.begin(), m_cIoDevice.GetCurData(), m_cIoDevice.GetCurData() + len);
	m_cIoDevice.Offset(len);
}

// 将数据放入streambuffer中
template<typename T>
inline void CSerializer::InputType(T t){
	int len = sizeof T;
	char* data = new char[len];
	const char* p = reinterpret_cast<const char*>(&t);
	memcpy(data, p, len);
	ChangeByteOrder(data, len);
	m_cIoDevice.Input(data, len);
	delete[] data;
}

template<>
inline void CSerializer::InputType(std::string _in)
{
	// 先存入字符串长度
	uint16_t len = _in.size();
	char* p = reinterpret_cast<char*>(&len);
	ChangeByteOrder(p, sizeof(uint16_t));
	m_cIoDevice.Input(p, sizeof(uint16_t));

	// 存入字符串
	if (len == 0) return;
	char* d = new char[len];
	memcpy(d, _in.c_str(), len);
	m_cIoDevice.Input(d, len);
	delete[] d;
}

template<>
inline void CSerializer::InputType(const char* _in)
{
	InputType<std::string>(std::string(_in));
}