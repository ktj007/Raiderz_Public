#include "stdafx.h"
#include "FBaseScriptCallback.h"
#include "GNPCScriptBuilder.h"

FBaseScriptCallback::FBaseScriptCallback()
{
	gsys.pScriptManager->Init();

	m_pFieldInfo = new GFieldInfo;
	m_pFieldInfo->m_nFieldID = 100;
	m_pFieldInfo->m_strFieldFile = L"100";
	m_pFieldInfo->SetFieldMaxXY_ForTest(50000.0f, 50000.0f);
	gmgr.pFieldInfoMgr->Insert(m_pFieldInfo);

	m_pField = GUTHelper_Field::NewMockField();
	m_pField->SetFieldInfo(m_pFieldInfo);
	gmgr.pFieldMgr->AddField(m_pField);

	m_pNPCInfo = new GNPCInfo;
	m_pNPCInfo->nID = 1;
	m_pNPCInfo->nMaxHP = 50000;
	wchar_t szBuff[128];
	swprintf_s(szBuff, L"NPC_%d", m_pNPCInfo->nID);
	m_pNPCInfo->strNameInLua = szBuff;
	gmgr.pNPCInfoMgr->Insert(m_pNPCInfo);
	
	GNPCScriptBuilder nsb(m_pNPCInfo);
	nsb.Build();
}

FBaseScriptCallback::~FBaseScriptCallback()
{
	m_pField->Update(1.0f);
	gmgr.pFieldMgr->DeleteField(m_pField->GetUID());

	gsys.pScriptManager->Fini();
}

GTalentInfo* FBaseScriptCallback::MakeSimpleDmgAtkTalent(const int nDamage, int nTalentID)
{
	if (nTalentID == INVALID_TALENT_ID)		nTalentID = INT_MAX-2;

	GTalentInfo* pAktTalent = gmgr.pTalentInfoMgr->Find(nTalentID);
	if (pAktTalent)			
	{
		m_Helper.SetTalentDamage(pAktTalent, nDamage);
		return pAktTalent;
	}

	pAktTalent = m_Helper.NewMeleeTalentInfo(nTalentID);
	m_Helper.SetTalentDamage(pAktTalent, nDamage);
	m_Helper.AddSimpleHitInfo(pAktTalent);

	return pAktTalent;
}

bool FBaseScriptCallback::RunLuaString( const char* szFormat, ... )
{
	char szBuff[1024];

	va_list args;
	va_start(args,szFormat);
	vsprintf(szBuff,szFormat,args);
	va_end(args);

	return WRUNSTRING(WLUA, szBuff);
}
