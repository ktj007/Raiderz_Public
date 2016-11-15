#include "StdAfx.h"
#include "GNPCInfoValidator.h"
#include "GSystem.h"
#include "GDef.h"
#include "GServerSystems.h"
#include "GNPCInfoMgr.h"
#include "GDialogInfoMgr.h"
#include "GNPCShopInfoMgr.h"
#include "GNPCShopInfo.h"
#include "GNPCInfo.h"
#include "GValidateLogger.h"
#include "CSStrings.h"
#include "CSDef_Interaction.h"

bool GNPCInfoValidator::Check()
{
	if (gmgr.pNPCInfoMgr == NULL)	return true;

	_VLI(FILENAME_NPCINFO);

	for (GNPCInfoMgr::iterator itor = gmgr.pNPCInfoMgr->begin(); itor != gmgr.pNPCInfoMgr->end(); ++itor)
	{
		GNPCInfo* pNPCInfo = static_cast<GNPCInfo*>((*itor).second);
		int nNPCID = (*itor).first;

		_ASSERT(NULL != pNPCInfo);
		if (NULL == pNPCInfo) continue;
		if (!pNPCInfo->bValidation) continue;

		_VLE(_T(NPC_XML_TAG_NPC));
		_VLA(_T(NPC_XML_ATTR_ID), nNPCID);
		_VLA(_T(NPC_XML_ATTR_NAME), pNPCInfo->strName);
		_VLP;

		_VLOGGER->ValidateID(nNPCID);

		CheckGrade(pNPCInfo->nGrade);
		/*CheckMeshName(pNPCInfo->strMeshName);*/
		CheckLevel(pNPCInfo->nLevel, pNPCInfo->nMaxLevel);
		CheckMaxHP(pNPCInfo->nMaxHP);
		CheckAP(pNPCInfo->nAP);
		CheckSpeed(pNPCInfo->fSpeed, pNPCInfo->fRunSpeed, pNPCInfo->fRotateSpeed);
		CheckSightRange(pNPCInfo->nSightRange);
		CheckScale(pNPCInfo->fScale);
		CheckMFModifier(pNPCInfo->MFModifier.Value);
		CheckInteraction(pNPCInfo->m_InteractionInfo, pNPCInfo);	
		CheckGather(pNPCInfo);
		CheckMode(pNPCInfo);
	}

	return (_VLOGGER->GetValidateResult() && GetReturnValue());
}

void GNPCInfoValidator::CheckGrade(int nGrade)
{
	_VLOGGER->ValidateMinus(nGrade, _T(NPC_XML_ATTR_GRADE));
}

void GNPCInfoValidator::CheckMeshName(const wstring& strMeshName)
{
	//if (NULL == gmgr.pNPCInfoMgr->GetHitInfo(strMeshName))
	//{
	//	m_ValidateLogger.Log(NPC_XML_ATTR_MESH_NAME "이 잘못 되어서 HitInfo를 얻을 수 없습니다.\n");
	//}
}

void GNPCInfoValidator::CheckLevel(int nMinLevel, int nMaxLevel)
{
	_VLOGGER->ValidateMinus(nMinLevel, _T(NPC_XML_ATTR_MIN_LEVEL));
	_VLOGGER->ValidateMinus(nMaxLevel, _T(NPC_XML_ATTR_MAX_LEVEL));
	_VLOGGER->ValidateBiggerThan(nMinLevel, nMaxLevel, _T(NPC_XML_ATTR_MIN_LEVEL), _T(NPC_XML_ATTR_MAX_LEVEL));
}

void GNPCInfoValidator::CheckMaxHP(int nMaxHP )
{
	_VLOGGER->ValidateMinus(nMaxHP, _T(NPC_XML_ATTR_MAX_HP));
}

void GNPCInfoValidator::CheckAP(int nAP)
{
	_VLOGGER->ValidateMinus(nAP, _T(NPC_XML_ATTR_AP));
}

void GNPCInfoValidator::CheckSpeed(float fSpeed, float fRunSpeed, float fRotateSpeed)
{
	_VLOGGER->ValidateMinus(fSpeed, _T(NPC_XML_ATTR_SPEED));
	_VLOGGER->ValidateMinus(fRunSpeed, _T(NPC_XML_ATTR_RUN_SPEED));	
	_VLOGGER->ValidateBiggerThan(fSpeed, fRunSpeed, _T(NPC_XML_ATTR_SPEED), _T(NPC_XML_ATTR_RUN_SPEED));
	_VLOGGER->ValidateMinus(fRotateSpeed, _T(NPC_XML_ATTR_ROTATE_SPEED));
}

void GNPCInfoValidator::CheckSightRange(int nSightRange)
{
	_VLOGGER->ValidateMinus(nSightRange, _T(NPC_XML_ATTR_SIGHTRANGE));
}

void GNPCInfoValidator::CheckScale(float fScale )
{
	_VLOGGER->ValidateMinus(fScale, _T(NPC_XML_ATTR_SCALE));
}

void GNPCInfoValidator::CheckMFModifier(PerMulVar<short> MotionFactors[MF_PRIORITY_SIZE])
{
	wchar_t szMFName[MF_PRIORITY_SIZE][128] = {	_T(NPC_XML_ATTR_BEATEN_MOD_VALUE), 
												_T(NPC_XML_ATTR_KNOCKBACK_MOD_VALUE),
												_T(NPC_XML_ATTR_STUN_MOD_VALUE),
												_T(NPC_XML_ATTR_DEFLECT_MOD_VALUE),
												_T(NPC_XML_ATTR_THROWUP_MOD_VALUE),
												_T(NPC_XML_ATTR_KNOCKDOWN_MOD_VALUE),
												_T(NPC_XML_ATTR_UPPERED_MOD_VALUE)};

	for (int mf = 0; mf < MF_PRIORITY_SIZE; mf++)
	{
		if (0 > MotionFactors[mf].fPer)
		{
			_VLOGGER->Log(PERCENT_S IS_MINUS, szMFName[mf]);
		}
		if (10.0f < MotionFactors[mf].fPer)
		{
			_VLOGGER->Log(PERCENT_S IS_TOO_BIG, szMFName[mf]);
		}
	}
}

void GNPCInfoValidator::CheckInteraction(const GNPCInteractionInfo& interactionInfo, GNPCInfo* pNPCInfo)
{
	if (true == interactionInfo.m_vecNPCIEInfo.empty()) return;
		
	for each (GNPCInteractionElementInfo* pIElementInfo in interactionInfo.m_vecNPCIEInfo)
	{
		if (NULL == pIElementInfo)
		{
			_ASSERT(0);
			continue;
		}

		wchar_t szIType[512];
		swprintf_s(szIType, 512, L"%s%d", _T(NPC_XML_ATTR_ITYPE), pIElementInfo->m_nID);
		wchar_t szICond[512];
		swprintf_s(szICond, 512, L"%s%d", _T(NPC_XML_ATTR_ICOND), pIElementInfo->m_nID);
		wchar_t szIAct[512];
		swprintf_s(szIAct, 512, L"%s%d", _T(NPC_XML_ATTR_IACT), pIElementInfo->m_nID);

		if (IT_MAILBOX != pIElementInfo->m_nType &&
			IT_STORAGE != pIElementInfo->m_nType &&
			IT_GUILD_STORAGE != pIElementInfo->m_nType&& 
			IT_GUILD_DESTROY != pIElementInfo->m_nType&& 
			IT_GUILD_CREATE != pIElementInfo->m_nType)
		{
			if (0 == pIElementInfo->GetSinlgeAct())
			{
				_VLL(L"IAct값이 기술 되어 있지 않습니다.");
			}
		}

		switch (pIElementInfo->m_nType)
		{
		case IT_DIALOG:
			{
				_VLOGGER->ValidateDialogID(pIElementInfo->GetSinlgeAct(), szIAct);
			}
			break;
		case IT_NPCSHOP:
			{	
				_VLOGGER->ValidateTradeID(pIElementInfo->GetSinlgeAct(), szIAct);
			}
			break;
		case IT_QUEST_END:
		case IT_QUEST_BEGIN:
			{
				_VLOGGER->ValidateQuestID(pIElementInfo->GetSinlgeAct(), szIAct);
			}
			break;
		case IT_QUEST_INTERACT_MORTAL:
		case IT_QUEST_INTERACT_IMMORTAL:
			{
				_VLOGGER->ValidateQuestID(pIElementInfo->GetSinlgeAct(), szIAct);

				if (1 >= pIElementInfo->m_vecAct.size())
				{
					_VLL(L"퀘스트 목적이 기술 되어 있지 않습니다.");
				}				
			}
			break;
		case IT_GATHER_1:
		case IT_GATHER_2:
		case IT_GATHER_3:
		case IT_GATHER_4:
		case IT_GATHER_5:
		case IT_GATHER_6:
		case IT_GATHER_7:
		case IT_GATHER_8:
		case IT_GATHER_9:
		case IT_GATHER_10:
		case IT_LOOT_MORTAL:
		case IT_LOOT_IMMORTAL:
			{
				_VLOGGER->ValidateLootID(pIElementInfo->GetSinlgeAct(), szIAct);
			}
			break;
		case IT_SOULBINDING:
			{
				_VLOGGER->ValidateSoulBindingID(pIElementInfo->GetSinlgeAct(), szIAct);
			}
			break;
		}
	}
}

void GNPCInfoValidator::CheckMode( GNPCInfo* pNPCInfo )
{
	if (pNPCInfo->vecModes.empty()) return;

	for (size_t i = 0; i < pNPCInfo->vecModes.size(); i++)
	{
		GNPCInfo* pChildNPCInfo = pNPCInfo->vecModes[i];

		if (NPC_MODE_DEFAULT > pChildNPCInfo->nMode)
		{
			_VLOGGER->Log(L"mode Info의 _T(MODE) 값이 제대로 설정되어 있지 않습니다 - NPC_MODE_DEFAULT보다 값이 낮다\n");
		}

		if (NPC_MODE_MAX <= pChildNPCInfo->nMode)
		{
			_VLOGGER->Log(L"mode Info의 _T(MODE) 값이 제대로 설정되어 있지 않습니다.  - NPC_MODE_MAX보다 값이 높다\n");
		}
	}

	if ((int)pNPCInfo->nSpawningMode > (int)pNPCInfo->vecModes.size())
	{
		_VLOGGER->Log(L"시작 모드(SpawningMode)값이 잘못되어 있습니다. (SpawningMode=%d)\n", (int)pNPCInfo->nSpawningMode);
	}
}

void GNPCInfoValidator::CheckGather(GNPCInfo* pNPCInfo)
{
	if (pNPCInfo->nNpcType != NPC_TYPE_GATHER)	return;

	if (pNPCInfo->nGatherType == GATHER_NONE)
	{		
		_VLOGGER->Log(L"%s이 없습니다.\n", _T(NPC_XML_ATTR_GATHER_TYPE));
	}
	
	for each(GNPCInteractionElementInfo* pInfo in pNPCInfo->m_InteractionInfo.m_vecNPCIEInfo)
	{
		if (pInfo->m_nID != 1)	continue;

		wchar_t szIType[512];
		swprintf_s(szIType, 512, L"%s%d", _T(NPC_XML_ATTR_ITYPE), pInfo->m_nID);
		wchar_t szICond[512];
		swprintf_s(szICond, 512, L"%s%d", _T(NPC_XML_ATTR_ICOND), pInfo->m_nID);
		wchar_t  szIAct[512];
		swprintf_s(szIAct, 512, L"%s%d", _T(NPC_XML_ATTR_IACT), pInfo->m_nID);

		if (pInfo->m_nType  < IT_GATHER_1 || IT_GATHER_10 < pInfo->m_nType)
		{
			_VLOGGER->Log(L"%s의 값이 gather_#이 아닙니다.\n", szIType);
		}
	}	
}
