#include "stdafx.h"
#include "XNPCInfoValidator.h"
#include "XNPCInfo.h"

bool XNPCInfoValidator::Check()
{
	bool ret = true;

	for (XNPCInfoMgr::iterator itor = info.npc->begin(); itor != info.npc->end(); ++itor)
	{
		XNPCInfo* pNPCInfo = static_cast<XNPCInfo*>((*itor).second);
		if (CheckTitle(pNPCInfo) == false) ret = false;
	}

	return ret;
}

bool XNPCInfoValidator::CheckTitle(XNPCInfo* pNPCInfo)
{
	if (pNPCInfo->bShowName && 
		pNPCInfo->strTitleBone.empty() && 
		pNPCInfo->fTitleHeight == 0.f)
	{
		_VLE(_T(NPC_XML_TAG_NPC));

		_VLA(_T(NPC_XML_ATTR_SHOW_NAME), L"true");
		_VLA(_T(NPC_XML_ATTR_TITLE_BONE), L"");
		_VLA(_T(NPC_XML_ATTR_TITLE_HEIGHT), L"0");

		_VLP;

		m_Logger.Log(L"타이틀 위치 에러(ID = %d)\n", pNPCInfo->nID);

		return false;
	}

	return true;
}

