#pragma once

#include "XReplay.h"
#include "XGlobalVar.h"

class XReplayAnalyzeParser
{
private:

public:
	XReplayAnalyzeParser() {}
	~XReplayAnalyzeParser() {}
};

class XReplayAnalyzer
{
private:
	GlobalVar_MyInfo		m_MyInfo;
	wstring					m_strReplayFileName;
	XReplayAnalyzeParser	m_Parser;
	XReplay*				m_pReplay;

	void SaveDPSReport(MXml& xmlAnalyze, const wchar_t* szFileName);
public:
	XReplayAnalyzer();
	~XReplayAnalyzer();
	bool Load(const wchar_t* szFileName);
	void Analyze();

	void SetCommands( MXmlElement* pRootElement );
	void SetCharInfo( MXmlElement* pRootElement, MXml* pXml );
	void SetReplayInfo( MXmlElement* pRootElement, MXml* pXml );
};