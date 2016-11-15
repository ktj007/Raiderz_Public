#include "StdAfx.h"
#include "XSoundResMgr.h"
#include "CCommandResultTable.h"
#include "XTalentAvailable.h"

XSoundResMgr::XSoundResMgr(void)
: CStringResMgr()
{	
	// do nothing
}

XSoundResMgr::~XSoundResMgr(void)
{
	for each(const result_map::value_type& result in m_mapResultStrings)
	{
		delete result.second;
	}	
}

XSoundResMgr* XSoundResMgr::GetInstance()
{
	static XSoundResMgr m_sMgr;
	return &m_sMgr;
}

bool XSoundResMgr::OnInit(const CLangNode& node)
{
	CStringRes<int>* pTable;

	result_map::iterator it = m_mapResultStrings.find(node.strLocale);

	if (it == m_mapResultStrings.end())
	{
		pTable = new CStringRes<int>(L"RESULT");
		m_mapResultStrings.insert(make_pair(node.strLocale, pTable));
	}
	else
	{
		pTable = it->second;
	}

	
	wstring strFileName = node.strPath + FILENAME_RESULT_SOUND;

	if (pTable->Load(strFileName.c_str()))
	{
		mlog("Error!! - %s Load Failed\n", MLocale::ConvUTF16ToAnsi(strFileName).c_str());
		return false;
	}

	return true;
}

const TCHAR* XSoundResMgr::GetCmdResultSound(int nKey)
{
	result_map::iterator it = m_mapResultStrings.find(m_strLocale);

	if (it == m_mapResultStrings.end())
	{
		return _T("");
	}

	CStringRes<int>* pTable = it->second;
	return pTable->GetStr(nKey);
}

void XSoundResMgr::PlayTalentResultSound( int nResult )
{
	switch (nResult)
	{
	case XTalentAvailable::RESULT_FALSE_BUFF:
		XCmdResultSound(CR_FAIL_USE_TALENT);
		break;
	case XTalentAvailable::RESULT_FALSE_COOLTIME:
		XCmdResultSound(CR_FAIL_USE_TALENT_NOTENOUGH_COOLDOWN);
		break;
	case XTalentAvailable::RESULT_FALSE_EN:
		XCmdResultSound(CR_FAIL_USE_TALENT_NOTENOUGH_EN);
		break;
	case XTalentAvailable::RESULT_FALSE_STA:
		XCmdResultSound(CR_FAIL_USE_TALENT_NOTENOUGH_STA);
		break;
	case XTalentAvailable::RESULT_FALSE_PORTALTALENT:
		XCmdResultSound(CR_FAIL_USE_TALENT_NOTUSE_IN_THISFIELD);
		break;
	}
}
