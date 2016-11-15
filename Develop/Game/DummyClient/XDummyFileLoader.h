#pragma once

#include "MXml.h"

class XDummyData;

class XDummyFileLoader
{
private:
	XDummyData*		m_pData;
	MXml			m_Xml;
	bool IsSameTag(MXmlElement* pElement, const char* szTagName);
	void ParseRoot(MXmlElement* pRootElement);
	void ParsePatterns(MXmlElement* pElement);
	void ParseAgents(MXmlElement* pElement);
	void ParseConfig(MXmlElement* pElement);
	void ParsePattern(MXmlElement* pElement);
	void ParseAgent(MXmlElement* pElement);
public:
	XDummyFileLoader(XDummyData* pOutData) : m_pData(pOutData) {}
	bool LoadFromStream(const wchar_t* szStream);
	bool Load(const wchar_t* szFileName);
};
