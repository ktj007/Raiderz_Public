#pragma once

class PLog
{
protected:
	wstring	m_strDate;

public:
	PLog(const wstring& strDate);
	virtual ~PLog();

	virtual wstring MakeQuery() = 0;
};

typedef vector<PLog*> VEC_LOG;

