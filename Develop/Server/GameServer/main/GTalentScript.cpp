#include "StdAfx.h"
#include "GTalentScript.h"
#include "GScriptManager.h"
#include "GDef.h"
#include "GGlobal.h"
#include "GField.h"
#include "GEntityActor.h"
#include "GTalent.h"
#include "GNullGlueActor.h"
#include "GScriptUtil.h"
#include "GEntityNPC.h"

//////////////////////////////////////////////////////////////////////////
//
//	GTalentScript
//
//////////////////////////////////////////////////////////////////////////

void GTalentScript::CallEventScript( GEntityActor* pActor, cscommon_server::TALENT_TARGET_INFO& infoTarget, const wstring& strText )
{
	PFI_B(3210, "GTalentScript::CallEventScript");

	if (strText.length() <= 0)					return;

	if (pActor->IsPlayer())
	{
		// 글로벌 함수를 호출
		if (!WLUA->IsExistGlobal(MLocale::ConvUTF16ToAnsi(strText.c_str()).c_str()))	return;

		VALID(pActor);
		GField* pField = pActor->GetField();
		VALID(pField);

		// 이벤트 스크립트 호출
		GEntityActor* pTarget = NULL;
		if (infoTarget.IsValid() && pField)	
		{
			pTarget = pField->FindActor(infoTarget());
		}

		WCALL2(WLUA, MLocale::ConvUTF16ToAnsi(strText.c_str()).c_str(), TO_SAFE_LUAPTR(pActor), TO_SAFE_LUAPTR(pTarget), WNULL);
	}
	else if (pActor->IsNPC()) 
	{
		GEntityNPC* pNPC = ToEntityNPC(pActor);
		VALID(pNPC);
		wstring strTable = pNPC->GetNPCInfo()->strNameInLua;
		if (!WLUA->IsExistMemberFunc(MLocale::ConvUTF16ToAnsi(strTable.c_str()).c_str(), MLocale::ConvUTF16ToAnsi(strText.c_str()).c_str()))	
			return;

		VALID(pActor);
		GField* pField = pActor->GetField();
		VALID(pField);

		// 이벤트 스크립트 호출
		GEntityActor* pTarget = NULL;
		if (infoTarget.IsValid() && pField)	
		{
			pTarget = pField->FindActor(infoTarget());
		}

		WMEMCALL2(WLUA, MLocale::ConvUTF16ToAnsi(strTable).c_str(), MLocale::ConvUTF16ToAnsi(strText).c_str(), TO_SAFE_LUAPTR(pActor), TO_SAFE_LUAPTR(pTarget), WNULL);
	}

	PFI_E(3210);
}

#include "GConst.h"
#include "GAIMonitorPort.h"

#define SEND_CALLBACK_MESSAGE_TO_VIEWER(EVENT, FUNC) \
	if (pTalent->GetOwner() && pTalent->GetOwner()->IsNPC()) { \
	GEntityNPC* pNPC = ToEntityNPC(pTalent->GetOwner());\
	VALID(pNPC);\
	wstring strTable = pNPC->GetNPCInfo()->strNameInLua; \
	if (GConst::AIMON_ENABLE && GAIMonitorPort::Inst().IsSetedNPC(pNPC->GetUID())) \
	if (WLUA->IsExistMemberFunc(MLocale::ConvUTF16ToAnsi(strTable).c_str(), MLocale::ConvUTF16ToAnsi(FUNC).c_str())) \
	GAIMonitorPort::Inst().SendCallbackMessageToViewer("%s (id: %d, func: %s:%s)", \
	EVENT, pTalent->GetID(), MLocale::ConvUTF16ToAnsi(strTable).c_str(), MLocale::ConvUTF16ToAnsi(FUNC).c_str()); }

void GTalentScript::OnStart(GTalent* pTalent)
{
	PFI_B(3220, "GTalentScript::OnStart");
	CallEventScript(pTalent->GetOwner(), pTalent->GetTarget(), pTalent->GetInfo()->m_strLuaOnStart);
	PFI_E(3220);

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnTalentStart", pTalent->GetInfo()->m_strLuaOnStart);
}

void GTalentScript::OnEnterPhaseAct( GTalent* pTalent )
{
	PFI_B(3230, "GTalentScript::OnEnterPhaseAct");
	CallEventScript(pTalent->GetOwner(), pTalent->GetTarget(), pTalent->GetInfo()->m_strLuaOnAct);
	PFI_E(3230);

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnTalentAct", pTalent->GetInfo()->m_strLuaOnAct);
}

void GTalentScript::OnLeavePhaseCanceled(GTalent* pTalent)
{
	PFI_B(3240, "GTalentScript::OnLeavePhaseCanceled");
	CallEventScript(pTalent->GetOwner(), pTalent->GetTarget(), pTalent->GetInfo()->m_strLuaOnCancel);
	PFI_E(3240);

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnTalentCancel", pTalent->GetInfo()->m_strLuaOnCancel);
}

void GTalentScript::OnLeavePhaseFinish(GTalent* pTalent)
{
	PFI_B(3250, "GTalentScript::OnLeavePhaseFinish");
	CallEventScript(pTalent->GetOwner(), pTalent->GetTarget(), pTalent->GetInfo()->m_strLuaOnFinish);
	PFI_E(3250);

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnTalentFinish", pTalent->GetInfo()->m_strLuaOnFinish);
}

void GTalentScript::OnDelayedAct(GTalent* pTalent)
{
	PFI_B(3250, "GTalentScript::OnDelayedAct");
	CallEventScript(pTalent->GetOwner(), pTalent->GetTarget(), pTalent->GetInfo()->m_strLuaOnDelayedAct);
	PFI_E(3250);

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnDelayedAct", pTalent->GetInfo()->m_strLuaOnDelayedAct);
}
