#pragma once

#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

// ����vector��ʵ���ֽ�������

class CStreamBuf : public vector<char> {
public:
	CStreamBuf() {
		// ��ʼ��
		m_iPos = 0;
	}
	CStreamBuf(const char* _in, size_t _len) {
		m_iPos = 0;
		insert(begin(), _in, _in + _len);
	}
	~CStreamBuf(){}
	// ����
	void Reset() {
		m_iPos = 0;
	}

	// ��ȡ��������
	const char* GetData() {
		return &(*this)[0];
	}

	// ��ȡ��ǰ����
	const char* GetCurData() {
		return &(*this)[m_iPos];
	}

	// ƫ��
	void Offset(int _k) {
		m_iPos += _k;
	}

	// �Ƿ����
	bool IsEOF() {
		return m_iPos >= size();
	}

	// ����
	void Input(char* _in, size_t _len) {
		insert(end(), _in, _in + _len);
	}

	// ����
	int FindChar(char _c) {
		iterator itr = find(begin() + m_iPos, end(), _c);
		if (itr != end())
		{
			return itr - (begin() + m_iPos);
		}
		return -1;
	}
private:
	int m_iPos;		// ��¼��ǰ�ֽ�����λ��
};


// ���л���
class CSerializer {
public:
	// Ĭ���ֽ���ΪС���ֽ���
	CSerializer() { m_eByteOrder = ByteOrder::Little; }
	~CSerializer() { }
	CSerializer(CStreamBuf _dev, int _byteOrder = 1) {
		m_eByteOrder = (ByteOrder)_byteOrder;
		m_cIoDevice = _dev;
	}
public:
	// �ֽ��򣬴��С��
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
	// ת���ֽ���
	void ChangeByteOrder(char* _in, int _len) {
		if (m_eByteOrder == ByteOrder::Big) {
			reverse(_in, _in + _len);
		}
	}
	// �������
	void WriteRawData(char* _in, int _len) {
		m_cIoDevice.Input(_in, _len);
		m_cIoDevice.Offset(_len);
	}

public:

private:
	ByteOrder m_eByteOrder;
	CStreamBuf m_cIoDevice;
};