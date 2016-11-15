#pragma once

#include "MMCode.h"


class XMMCode
{
private:
	mmcode::MTagManager		m_TagManager;
	void InitControlCode();
	void InitReplaceChars();
public:
	XMMCode();
	~XMMCode(){}
	void Init();

	mmcode::MTagManager& GetTagManager() { return m_TagManager; }
};


class XMMCodeMgr
{
private:
	XMMCode		m_MMCode;
public:
	XMMCodeMgr()
	{
		m_MMCode.Init();
	}
	static XMMCodeMgr& GetInstance();
	mmcode::MTagManager& GetTagManager() { return m_MMCode.GetTagManager(); }

	// 문자열에 속한 제어코드만 실행
	void ExecControlCode(const wchar_t* szKey, MUID uidOwner);

	// 치환자를 치환하고 제어코드까지 번역해서 반환. 다만 제어코드의 실행은 하지 않는다.
	mmcode::MControlCodeTransResult TransControlCodeAndReplaceChars(const wchar_t* szKey, MUID uidOwner);

	//문자열에 속한 코드를 기준으로 스트링을 나눈다.
	void TransControlSpliterChars(list<string>& strlist, const char* szKey, MUID uidOwner );
};