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