#include "stdafx.h"
#include "FBaseScriptGlue.h"
#include "GGlobal.h"
#include "GFieldInfoMgr.h"
#include "GUTHelper_Field.h"
#include "MockField.h"
#include "GTalentInfoMgr.h"

FBaseScriptGlue::FBaseScriptGlue()
{
	gsys.pScriptManager->Init();

	m_pFieldInfo = new GFieldInfo;
	m_pFieldInfo->m_nFieldID = 100;
	m_pFieldInfo->m_bAILODEnable = false;
	m_pFieldInfo->SetFieldMaxXY_ForTest(50000.0f, 50000.0f);
	{
		m_SensorInfo.nID = 777;
		m_SensorInfo.bStartingEnable = true;
		m_pFieldInfo->InsertSensor(m_SensorInfo);
	}
	gmgr.pFieldInfoMgr->Insert(m_pFieldInfo);

	m_pField = GUTHelper_Field::NewMockField(m_pFieldInfo);
	gmgr.pFieldMgr->AddField(m_pField);

	m_pNPCInfo = new GNPCInfo;
	m_pNPCInfo->nID = 1;
	m_pNPCInfo->nMaxHP = 50000;
	wchar_t szBuff[128];
	swprintf_s(szBuff, L"NPC_%d", m_pNPCInfo->nID);
	m_pNPCInfo->strNameInLua = szBuff;
	gmgr.pNPCInfoMgr->Insert(m_pNPCInfo);
}

FBaseScriptGlue::~FBaseScriptGlue()
{
	m_pField->Update(1.0f);
	m_pField->Destroy();
	gmgr.pFieldMgr->DeleteField(m_pField->GetUID());
	gmgr.pFieldMgr->Clear();

	gsys.pScriptManager->Fini();
}

GTalentInfo* FBaseScriptGlue::MakeSimpleDmgAtkTalent(const int nDamage, int nTalentID)
{
	if (nTalentID == INVALID_TALENT_ID)		nTalentID = INT_MAX-2;

	GTalentInfo* pAtkTalent = gmgr.pTalentInfoMgr->Find(nTalentID);
	if (pAtkTalent)			
	{
		m_Helper.SetTalentDamage(pAtkTalent, nDamage);
		return pAtkTalent;
	}

	pAtkTalent = m_Helper.NewMeleeTalentInfo(nTalentID);
	m_Helper.SetTalentDamage(pAtkTalent, nDamage);
	m_Helper.AddSimpleHitInfo(pAtkTalent);

	return pAtkTalent;
}
