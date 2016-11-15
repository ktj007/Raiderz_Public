#include "StdAfx.h"
#include "XCmdHandler_Training.h"
#include "XTalentInfo.h"
#include "XMyPlayer.h"
#include "XPost_Interaction.h"
#include "XStrings.h"
#include "XTalentInfoMgr.h"

//TALENT_ID_LIST g_TalentIDList;

XCmdHandler_Training::XCmdHandler_Training(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_TRAINING_TRAIN,			OnTrain);
	SetCmdHandler(MC_TRAINING_UNTRAIN_ALL,		OnUntrainAll);
}

MCommandResult XCmdHandler_Training::OnTrain(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nTalentID;
	bool bConsumeTP;

	if (pCommand->GetParameter(&nTalentID,	0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&bConsumeTP, 1, MPT_BOOL)==false) return CR_ERROR;

	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	_ASSERT(NULL != pTalentInfo && "pTalentInfo is NULL");
	if (NULL == pTalentInfo) return CR_ERROR;

	bool bTrainNewExtraPassive1 = !gvar.MyInfo.Talent.HasExtraPassiveTalent(pTalentInfo->m_nExtraPassive);
	bool bTrainNewExtraPassive2 = !gvar.MyInfo.Talent.HasExtraPassiveTalent(pTalentInfo->m_nExtraPassive2);

	// 탤런트 학습
	if (false == gvar.MyInfo.Talent.Train(pTalentInfo, bConsumeTP)) return CR_ERROR;

	global.ui->OnSystemMsg( XGetStr(L"TALENT_LEARNED"), FSParam(pTalentInfo->GetName()));
	global.ui->OnLearnTalentEffect();
	global.ui->TalentRefresh();

	if(!pTalentInfo->IsPassiveSkill())
		global.ui->SetPaletteTalent(nTalentID);

	// 라이센스가 필요한 장비효과 적용
	if (bTrainNewExtraPassive1)
	{
		gvar.MyInfo.EquipmentSlot.AddModifierByTrain(pTalentInfo->m_nExtraPassive);		
	}
	if (bTrainNewExtraPassive2)
	{
		gvar.MyInfo.EquipmentSlot.AddModifierByTrain(pTalentInfo->m_nExtraPassive2);
	}
	
	if (global.script)
	{
		global.script->GetGlueGameEvent().OnGameEvent( "CHARINFO", "STATUS");
		global.script->GetGlueGameEvent().OnGameEvent( "CHARINFO", "TALENT");
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Training::OnUntrainAll(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nTotalTP;
	if (pCommand->GetParameter(&nTotalTP,	0, MPT_INT)==false) return CR_ERROR;

	gvar.MyInfo.Talent.UntrainAll();
	// 운영자 명령어로 획득한 TP를 제거한 순수한 TP로 세팅
	gvar.MyInfo.Talent.SetTP(nTotalTP);

	global.ui->OnSystemMsg( XGetStr( L"TALENT_RESETALL"));
	global.ui->OnUnTalentEffect();

	// PaletteTalentList 초기화 시켜주기
	gvar.MyInfo.PaletteList.DeleteAllItem();

	// 라이센스가 필요한 장비효과 미적용
	gvar.MyInfo.EquipmentSlot.EraseModifierByUntrain();

	//ui갱신	
	global.ui->TalentRefresh();
	global.ui->PaletteUIRefresh();
	if (global.script)
	{
		global.script->GetGlueGameEvent().OnGameEvent( "CHARINFO", "STATUS");
		global.script->GetGlueGameEvent().OnGameEvent( "CHARINFO", "TALENT");
	}

	return CR_TRUE;
}