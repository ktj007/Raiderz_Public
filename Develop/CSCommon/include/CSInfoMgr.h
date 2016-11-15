#pragma once

#include "CSCommonLib.h"

class CSInfo;

typedef map<int, CSInfo*>	MAP_INFO;

class CSCOMMON_API CSInfoMgr
{
private:
	MAP_INFO		m_mapInfo;
	static string	m_strMaiet;

protected:
	virtual bool Parse(MXmlElement* pElement, MXml* pXml, CSInfo* pInfo) { return true; }
	CSInfo*	Get(int nID);

public:
	CSInfoMgr();
	virtual ~CSInfoMgr();

	void Clear();

	bool Load(const TCHAR* szFileName, const TCHAR* szTagName);
	virtual CSInfo* New() = 0;
	void Insert(CSInfo* pInfo);
	
	bool IsExist(int nID);
};