#include "StdAfx.h"
#include "TStringResMgr.h"

TStringResMgr::TStringResMgr(void)
{
}

TStringResMgr::~TStringResMgr(void)
{
}

TStringResMgr* TStringResMgr::GetInstance()
{
	static TStringResMgr m_instance;
	return &m_instance;
}

bool TStringResMgr::BeginQuestTool(String^ strPath)
{
	if (!m_StrQuestToolTable.Initialize(MStringToCharPointer(strPath)))
	{
		_ASSERT(0);
		mlog("Error!! - StrQuestToolTable Initalize Failed\n");
		return false;
	}

	return true;
}

void TStringResMgr::EndQuestTool()
{
	m_StrQuestToolTable.Release();
}

String^ TStringResMgr::GetStringValue(String^ strKey)
{
	String^ strValue = nullptr;
	string key = MStringToCharPointer(strKey);

	strValue = gcnew String(CStringResMgr::GetString(key));
	if (nullptr != strValue) return strValue;

	strValue = this->GetStringFromStrQuestToolTable(strKey);
	if (nullptr != strValue) return strValue;

	return nullptr;
}

String^ TStringResMgr::GetStringFromStrQuestToolTable(String^ strKey)
{
	String^ strValue = gcnew String(m_StrQuestToolTable.GetStr(MStringToCharPointer(strKey)));

	return strValue;
}

bool TStringResMgr::BeginQuest(String^ strPath)
{
	if (!m_StrQuestTable.Initialize(MStringToCharPointer(strPath)))
	{
		_ASSERT(0);
		mlog("Error!! - StrQuestTable Initalize Failed\n");
		return false;
	}

	return true;
}

void TStringResMgr::EndQuest()
{
	m_StrQuestTable.Release();
}

bool TStringResMgr::BeginDialog(String^ strPath)
{
	String^ strOnScreedDialogPath = String::Format("{0}{1}", strPath, FILENAME_STRINGS_ONSCREEN);
	if (!m_StrOnScreenDialogTable.Initialize(MStringToCharPointer(strOnScreedDialogPath)))
	{
		_ASSERT(0);
		mlog("Error!! - StrOnScreenDialogTable Initalize Failed\n");
		return false;
	}

	String^ strOffScreedDialogPath = String::Format("{0}{1}", strPath, FILENAME_STRINGS_OFFSCREEN);
	if (!m_StrOffScreenDialogTable.Initialize(MStringToCharPointer(strOffScreedDialogPath)))
	{
		_ASSERT(0);
		mlog("Error!! - StrOffScreenDialogTable Initalize Failed\n");
		return false;
	}

	return true;
}

void TStringResMgr::EndDialog()
{
	m_StrOnScreenDialogTable.Release();
	m_StrOffScreenDialogTable.Release();
}

