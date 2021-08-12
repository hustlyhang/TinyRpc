#pragma once

#include <vector>
#include <sstream>
#include <algorithm>

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
		return m_iPos >= size();
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

private:
	ByteOrder m_eByteOrder;
	CStreamBuf m_cIoDevice;
};