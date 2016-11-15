#pragma once

class XHistory
{
private:
	map<wstring, wstring>		m_Values;
public:
	XHistory() {}
	bool Load(const wchar_t* szFileName);
	bool Save(const wchar_t* szFileName);
	wstring GetValue(const wchar_t* szKey);
	int GetValueNum(const wchar_t* szKey);
	void SetValue(const wchar_t* szKey, const wchar_t* szValue);
	void SetValue(const wchar_t* szKey, int nValue);
};