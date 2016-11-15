#include "stdafx.h"
#include "XCmdHandler_Talent.h"
#include "XModuleEntity.h"
#include "XModuleActorControl.h"
#include "XTalentInfo.h"
#include "XBuffInfo.h"
#include "XNonPlayer.h"
#include "XTalentInfoMgr.h"
#include "XModuleEffect.h"
#include "XStrings.h"

XCmdHandler_Talent::XCmdHandler_Talent(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_BUFF_GAIN,						OnBuffGain);
	SetCmdHandler(MC_BUFF_LOST,						OnBuffLost);
	SetCmdHandler(MC_BUFF_INSTANT_EFFECT_GAIN,		OnBuffInstantEffectGain);
	SetCmdHandler(MC_TALENT_INSTANT_EFFECT_GAIN,	OnTalentInstantEffectGain);
	SetCmdHandler(MC_FOCUS_GAIN,					OnFocusGain);
	SetCmdHandler(MC_FOCUS_LOST,					OnFocusLost);
	SetCmdHandler(MC_FOCUS_UPDATE_BUBBLE,			OnFocusUpdateBubble);
	SetCmdHandler(MC_BUFF_HEAL,						OnBuffHeal);
	SetCmdHandler(MC_BUFF_STACK_INCREASE,			OnBuffIncrease);
	SetCmdHandler(MC_BUFF_STACK_DECREASE,			OnBuffDecrease);
}

MCommandResult XCmdHandler_Talent::OnBuffGain(MCommand* pCommand, MCommandHandler* pHandler)
{
	UIID nTargetUIID;
	int nBuffID;
	float fLostRemained;
	if (pCommand->GetParameter(&nTargetUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nBuffID,		1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&fLostRemained,	2, MPT_FLOAT)==false) return CR_ERROR;

	XObject* pObject = gg.omgr->FindActor_UIID(nTargetUIID);
	if (NULL == pObject) return CR_TRUE;
	
	XModuleActorControl* pModuleActorControl = pObject->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnBuffTalentGain(nBuffID, fLostRemained);
	}

	if (global.script)
		global.script->GetGlueGameEvent().OnGameEvent( "CHARINFO", "STATUS");

	return CR_TRUE;
}

MCommandResult XCmdHandler_Talent::OnBuffLost(MCommand* pCommand, MCommandHandler* pHandler)
{
	UIID nTargetUIID;
	int nBuffID;

	if (pCommand->GetParameter(&nTargetUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nBuffID,		1, MPT_INT)==false) return CR_ERROR;

	if(gg.omgr == NULL)  return CR_TRUE;

	XObject* pObject = gg.omgr->FindActor_UIID(nTargetUIID);
	if (NULL == pObject) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pObject->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnBuffTalentLost(nBuffID);
	}

	if (global.script)
		global.script->GetGlueGameEvent().OnGameEvent( "CHARINFO", "STATUS");

	return CR_TRUE;
}

MCommandResult XCmdHandler_Talent::OnFocusGain(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidTarget;
	TALENT_FOCUS_TYPE nFocusType;
	float fDurationTime;
	if (pCommand->GetParameter(&uidTarget,		0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nFocusType,		1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&fDurationTime,	2, MPT_FLOAT)==false) return CR_ERROR;

	// 임시 문자열 테이블
	map<TALENT_FOCUS_TYPE, wstring> mapTempString;
	mapTempString[TFT_COUNTER] = XGetStr(L"FOCUS_DEFENDER");
	mapTempString[TFT_BERSERK] = XGetStr(L"FOCUS_BERSERKER");
	mapTempString[TFT_SNEAK] = XGetStr(L"FOCUS_ASSASSIN");
	mapTempString[TFT_PRECISION] = XGetStr(L"FOCUS_RANGER");
	mapTempString[TFT_ENLIGHTEN] = XGetStr(L"FOCUS_SORCERER");
	mapTempString[TFT_ADVENT] = XGetStr(L"FOCUS_CLERIC");
	// ------------------


	XObject* pObject = gg.omgr->Find(uidTarget);
	if (NULL == pObject) return CR_TRUE;

	// 전투 메세지 분리 _by tripleJ 110504
	if (XGetMyUID() == uidTarget)
	{
		global.ui->OnBattleMsg( XGetStr( L"TALENT_GETFOCUSEFFECT"), FSParam(mapTempString[nFocusType]));
	}
	else
	{
		global.ui->OnBattleMsg( XGetStr( L"TALENT_GETFOCUSEFFECTOTHER"), FSParam(pObject->GetName(), mapTempString[nFocusType]));
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Talent::OnFocusLost(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidTarget;
	TALENT_FOCUS_TYPE nFocusType;
	if (pCommand->GetParameter(&uidTarget,		0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nFocusType,		1, MPT_INT)==false) return CR_ERROR;

	// 임시 문자열 테이블
	map<TALENT_FOCUS_TYPE, wstring> mapTempString;
	mapTempString[TFT_COUNTER] = XGetStr(L"FOCUS_DEFENDER");
	mapTempString[TFT_BERSERK] = XGetStr(L"FOCUS_BERSERKER");
	mapTempString[TFT_SNEAK] = XGetStr(L"FOCUS_ASSASSIN");
	mapTempString[TFT_PRECISION] = XGetStr(L"FOCUS_RANGER");
	mapTempString[TFT_ENLIGHTEN] = XGetStr(L"FOCUS_SORCERER");
	mapTempString[TFT_ADVENT] = XGetStr(L"FOCUS_CLERIC");
	// ------------------

	XObject* pObject = gg.omgr->Find(uidTarget);
	if (NULL == pObject) return CR_TRUE;

	// 전투 메세지 분리 _by tripleJ 110504
	if (XGetMyUID() == uidTarget)
	{
		global.ui->OnBattleMsg( XGetStr( L"TALENT_LOSTFOCUSEFFECT"), FSParam(mapTempString[nFocusType]));
	}
	else
	{
		global.ui->OnBattleMsg( XGetStr( L"TALENT_LOSTFOCUSEFFECTOTHER"), FSParam(pObject->GetName(), mapTempString[nFocusType]));
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Talent::OnFocusUpdateBubble(MCommand* pCommand, MCommandHandler* pHandler)
{
	TALENT_FOCUS_TYPE	nFocusType;
	uint8				nBubblePercentage;
	if (pCommand->GetParameter(&nFocusType,		0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nBubblePercentage,		1, MPT_UCHAR)==false) return CR_ERROR;

	// 임시 문자열 테이블
	map<TALENT_FOCUS_TYPE, wstring> mapTempString;
	mapTempString[TFT_COUNTER] = XGetStr(L"FOCUS_DEFENDER");
	mapTempString[TFT_BERSERK] = XGetStr(L"FOCUS_BERSERKER");
	mapTempString[TFT_SNEAK] = XGetStr(L"FOCUS_ASSASSIN");
	mapTempString[TFT_PRECISION] = XGetStr(L"FOCUS_RANGER");
	mapTempString[TFT_ENLIGHTEN] = XGetStr(L"FOCUS_SORCERER");
	mapTempString[TFT_ADVENT] = XGetStr(L"FOCUS_CLERIC");

	// ------------------


	// 전투 메세지 분리 _by tripleJ 110504
	wstring strResult = FormatString(XGetStr(L"GAIN_BUBBLE"), FSParam(mapTempString[nFocusType].c_str(), nBubblePercentage));
	global.ui->OnBattleMsg(strResult.c_str());

	//gvar.MyInfo.BubblePercent.insert(map<TALENT_FOCUS_TYPE, int>::value_type(nFocusType, nBubblePercentage));
	gvar.MyInfo.nFocusType = nFocusType;
	gvar.MyInfo.nBubblePercent = nBubblePercentage;

	return CR_TRUE;
}


MCommandResult XCmdHandler_Talent::OnTalentInstantEffectGain(MCommand* pCommand, MCommandHandler* pHandler)
{
	UIID nTargetUIID;
	int nTalentID;

	if (pCommand->GetParameter(&nTargetUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nTalentID,		1, MPT_INT)==false) return CR_ERROR;

	XTalentInfo* pInfo = info.talent->Get(nTalentID);
	if (pInfo == NULL)	return CR_ERROR;

	XObject* pObject = gg.omgr->FindActor_UIID(nTargetUIID);
	if (pObject == NULL) return CR_ERROR;

	XModuleActorControl* pModuleActorControl = pObject->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnTalentInstantEffectGain(pInfo);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Talent::OnBuffInstantEffectGain(MCommand* pCommand, MCommandHandler* pHandler)
{
	UIID nTargetUIID;
	int nBuffID;

	if (pCommand->GetParameter(&nTargetUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nBuffID,		1, MPT_INT)==false) return CR_ERROR;

	XBuffInfo* pInfo = info.buff->Get(nBuffID);
	if (pInfo == NULL)	return CR_ERROR;

	XObject* pObject = gg.omgr->FindActor_UIID(nTargetUIID);
	if (pObject == NULL) return CR_ERROR;

	XModuleActorControl* pModuleActorControl = pObject->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnBuffInstantEffectGain(nBuffID);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Talent::OnBuffHeal(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nUserUIID;
	UIID nTargetUIID;
	int nBuffID;
	int nAmount;
	if (pCommand->GetParameter(&nUserUIID,		0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nTargetUIID,	1, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nBuffID,		2, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nAmount,		3, MPT_INT)==false) return CR_ERROR;

	XActor* pActor = gg.omgr->FindActor_UIID(nTargetUIID);
	if (!pActor) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pActor->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnBuffHeal(nUserUIID, nBuffID, nAmount);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Talent::OnBuffIncrease(MCommand* pCommand, MCommandHandler* pHandler)
{
	UIID nTargetUIID;
	int nBuffID;
	float fLostRemained;
	if (pCommand->GetParameter(&nTargetUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nBuffID,		1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&fLostRemained,	2, MPT_FLOAT)==false) return CR_ERROR;

	XObject* pObject = gg.omgr->FindActor_UIID(nTargetUIID);
	if (NULL == pObject) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pObject->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnBuffIncrease(nBuffID, fLostRemained);
	}

	if (global.script)
		global.script->GetGlueGameEvent().OnGameEvent( "CHARINFO", "STATUS");

	return CR_TRUE;
}

MCommandResult XCmdHandler_Talent::OnBuffDecrease(MCommand* pCommand, MCommandHandler* pHandler)
{
	UIID nTargetUIID;
	int nBuffID;

	if (pCommand->GetParameter(&nTargetUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nBuffID,		1, MPT_INT)==false) return CR_ERROR;

	if(gg.omgr == NULL)  return CR_TRUE;

	XObject* pObject = gg.omgr->FindActor_UIID(nTargetUIID);
	if (NULL == pObject) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pObject->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnBuffDecrease(nBuffID);
	}

	if (global.script)
		global.script->GetGlueGameEvent().OnGameEvent( "CHARINFO", "STATUS");

	return CR_TRUE;
}
