#include "stdafx.h"
#include "XGlueGameEvent.h"
#include "XGlueFunctions.h"

XScript::XScript() : m_pLua(NULL)
{

}

XScript::~XScript()
{
	SAFE_DELETE(m_pLua);
}

void XScript::Init()
{
	if (m_pLua == NULL) m_pLua = new MWLua();
	m_pLua->Open();
}

bool XScript::Create()
{
	if (m_pLua == NULL) m_pLua = new MWLua();

	m_GlueFunctions.Init(m_pLua);
	m_GlueCharEvent.Init(m_pLua);
	m_GlueGameEvent.Init(m_pLua);
	m_GlueDebugEvent.Init(m_pLua);
	m_GlueFieldEvent.Init(m_pLua);
	m_GlueCutSceneEvent.Init(m_pLua);

	// 디버그용 스크립트 셋팅
#ifndef _PUBLISH
	SetDebugScript();
#endif

	return true;
}

void XScript::Destroy()
{
	if (m_pLua)
	{
		m_pLua->Close();
		SAFE_DELETE(m_pLua);
	}
}


void XScript::ErrorHandler(const wchar_t* szErrMessage)
{
	if (global.ui)
		global.ui->OutputToConsole(szErrMessage);
}



// madduck [5/11/2006]
void XScript::Event(const wchar_t* szEvent)
{
	WCALL(m_pLua, MLocale::ConvUTF16ToAnsi(szEvent).c_str(), WNULL);
}

void XScript::Event(const wchar_t* szEvent, const wchar_t* str)
{
	WCALL1(m_pLua, MLocale::ConvUTF16ToAnsi(szEvent).c_str(), str, WNULL);
}

void XScript::Event(const wchar_t* szEvent, const wchar_t* str1, const wchar_t* str2)
{
	WCALL2(m_pLua, MLocale::ConvUTF16ToAnsi(szEvent).c_str(), MLocale::ConvUTF16ToAnsi(str1).c_str(), MLocale::ConvUTF16ToAnsi(str2).c_str(), WNULL);
}

void XScript::Event(const wchar_t* szEvent, const wchar_t* str1, const wchar_t* str2, const wchar_t* str3)
{
	WCALL3(m_pLua, MLocale::ConvUTF16ToAnsi(szEvent).c_str(), MLocale::ConvUTF16ToAnsi(str1).c_str(), MLocale::ConvUTF16ToAnsi(str2).c_str(), MLocale::ConvUTF16ToAnsi(str3).c_str(), WNULL);
}

void XScript::Event(const wchar_t* szEvent, double fArg1)
{
	WCALL1(m_pLua, MLocale::ConvUTF16ToAnsi(szEvent).c_str(), fArg1, WNULL);
}

void XScript::Event(const wchar_t* szEvent, double fArg1, double fArg2)
{
	WCALL2(m_pLua, MLocale::ConvUTF16ToAnsi(szEvent).c_str(), fArg1, fArg2, WNULL);
}

void XScript::Event(const wchar_t* szEvent, double fArg1, double fArg2, double fArg3)
{
	WCALL3(m_pLua, MLocale::ConvUTF16ToAnsi(szEvent).c_str(), fArg1, fArg2, fArg3, WNULL);
}


void XScript::RunString(const wchar_t* szString)
{
	if (m_pLua == NULL) return;
	m_pLua->RunString(MLocale::ConvUTF16ToAnsi(szString).c_str(), strlen(MLocale::ConvUTF16ToAnsi(szString).c_str()), WLUA_DEBUG_ARG);
}

bool XScript::RunFile(const wchar_t* szFileName)
{
	if (m_pLua == NULL) return false;
	return m_pLua->RunFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str(), WLUA_DEBUG_ARG);
}

void XScript::SetDebugScript()
{
	bool bDebugScript = false;
	bDebugScript = RunFile(FILENAME_SCRIPT_DEBUG);
	if (bDebugScript == false)
	{
		bDebugScript = RunFile(FILENAME_SCRIPT_DEBUG_REL);
	}

	if(bDebugScript == false)
		return;
}
