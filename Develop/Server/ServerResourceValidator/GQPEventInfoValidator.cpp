#include "StdAfx.h"
#include "GQPEventInfoValidator.h"
#include "GGlobal.h"
#include "GDef.h"
#include "GValidateLogger.h"
#include "GQPEventInfoMgr.h"
#include "GQPEventInfo.h"
#include "CSDef_QuestPVP.h"

bool GQPEventInfoValidator::Check()
{
	if (gmgr.pQPEventInfoMgr == NULL)	return true;

	_VLI(FILENAME_QPEVENT);

	vector<GQPEventInfo*> vecInfo = gmgr.pQPEventInfoMgr->GetAll();
	for each(GQPEventInfo* pInfo in vecInfo)
	{
		CheckEvent(pInfo);
	}

	return (_VLOGGER->GetValidateResult() && GetReturnValue());
}

void GQPEventInfoValidator::CheckEvent( GQPEventInfo* pInfo )
{
	VALID(pInfo);

	_VLE(_T(QPEVENT_XML_TAG_QPEVENT));	
	_VLA(_T(QPEVENT_XML_ATTR_ID), pInfo->m_nID);
	_VLA(_T(QPEVENT_XML_ATTR_TITLE), pInfo->m_strTitle);
	_VLP;

	_VLOGGER->ValidateID(pInfo->m_nID);
	_VLOGGER->ValidateEmptyText(pInfo->m_strTitle, _T(QPEVENT_XML_ATTR_TITLE));
	_VLOGGER->ValidateLengthText(pInfo->m_strTitle, QPE_TITLE_LEN, _T(QPEVENT_XML_ATTR_TITLE));

	CheckBegin(pInfo->m_BeginInfo);

	for each (GQPEEndInfo* pEndInfo in pInfo->m_vecEndInfo)
	{
		CheckEnd(pEndInfo);
	}
}

void GQPEventInfoValidator::CheckBegin( const GQPEBeginInfo &beginInfo )
{
	_VLE(_T(QPEVENT_XML_TAG_BEGIN));
	_VLP;

	for each (const wstring& strNotice in beginInfo.m_vecStrNotice)
	{
		CheckNotice(strNotice);
	}

	for each (GQPESpawnInfo* pSpawnInfo in beginInfo.m_vecSpawnInfo)
	{
		CheckSpawn(pSpawnInfo);
	}
}

void GQPEventInfoValidator::CheckEnd( GQPEEndInfo* pEndInfo )
{
	_VLE(_T(QPEVENT_XML_TAG_END));
	_VLP;

	for each (const wstring& strNotice in pEndInfo->m_vecStrNotice)
	{
		CheckNotice(strNotice);
	}

	for each (GQPESpawnInfo* pSpawnInfo in pEndInfo->m_vecSpawnInfo)
	{
		CheckSpawn(pSpawnInfo);
	}

	CheckDespawn(pEndInfo->m_vecDespawnNPCID);
	CheckReward(pEndInfo);	
}

void GQPEventInfoValidator::CheckNotice( wstring strNotice )
{
	_VLE(_T(QPEVENT_XML_TAG_NOTICE));
	_VLP;

	_VLOGGER->ValidateEmptyText(strNotice, _T(QPEVENT_XML_ATTR_TEXT));
	_VLOGGER->ValidateLengthText(strNotice, QPE_NOTICE_LEN, _T(QPEVENT_XML_ATTR_TEXT));
}

void GQPEventInfoValidator::CheckSpawn( GQPESpawnInfo* pSpawnInfo )
{
	_VLE(_T(QPEVENT_XML_TAG_SPAWN));
	_VLP;

	_VLOGGER->ValidateMinus(pSpawnInfo->m_fDelayTime, _T(QPEVENT_XML_ATTR_DELAYTIME));
	_VLOGGER->ValidateMinus(pSpawnInfo->m_fDespawnTime, _T(QPEVENT_XML_ATTR_DESPAWNTIME));	
}

void GQPEventInfoValidator::CheckDespawn( const vector<int>& vecDespawnNPCID )
{
	_VLE(_T(QPEVENT_XML_TAG_DESPAWN));
	_VLP;

	for each (int nDespawnNPCID in vecDespawnNPCID)
	{
		_VLOGGER->ValidateNPCID(nDespawnNPCID, _T(QPEVENT_XML_ATTR_NPCID));
	}
}

void GQPEventInfoValidator::CheckReward( GQPEEndInfo* pEndInfo )
{
	_VLE(_T(QPEVENT_XML_TAG_REWARD));
	_VLP;

	_VLOGGER->ValidateMinus(pEndInfo->m_nMoney, _T(QPEVENT_XML_ATTR_MONEY));
	_VLOGGER->ValidateMinus(pEndInfo->m_nXP, _T(QPEVENT_XML_ATTR_XP));

	for each (GQPEItemInfo* pItemInfo in pEndInfo->m_vecItemInfo)
	{
		_VLE(_T(QPEVENT_XML_TAG_ITEM));
		_VLP;

		_VLOGGER->ValidateItemID(pItemInfo->m_nItemID, _T(QPEVENT_XML_ATTR_ID));
		_VLOGGER->ValidateMinusORZero(pItemInfo->m_nAmount, _T(QPEVENT_XML_ATTR_AMOUNT));
	}
}

