#include "stdafx.h"
#include "XMMCodeProc.h"
#include "XModuleMotion.h"
#include "XModuleTalent.h"
#include "XModuleBuff.h"
#include "XNonPlayer.h"
#include "XPlayer.h"
#include "XStrings.h"
#include "XBuffAttribute.h"

void XAniControlCodeProc::_Proc( const string& strAniName, MUID uidOwner )
{
	XActor* pActor = gg.omgr->FindActor(uidOwner);
	if (pActor == NULL) return;

	// 현재 모션을 기억을한다.
	wstring strPreMotion = pActor->GetModuleMotion()->GetCurrMotion();
	wstring strPreAniName;
	if(info.motion->GetMotion(strPreMotion.c_str()))
	{
		strPreAniName = pActor->GetModuleMotion()->GetCustomMotionAniName(strPreMotion);
	}


	//
	pActor->GetModuleMotion()->ChangeToCustomMotion(MOTION_NAME_CUSTOM, MLocale::ConvAnsiToUTF16(strAniName.c_str()).c_str(), true);

	if (pActor->GetEntityType() == ETID_NPC)
	{
		// npc가 기절/루트에 걸렸나?
		//XBuffAttribute attrBuff = pActor->GetModuleBuff()->GetBuffAttribute();
		//if ( attrBuff.IsMesmerizable())
		//{
		//	// 그랬다면 기절/루트 애니메이션으로 돌아가야 한다.
		//	if(strPreAniName.empty())
		//	{
		//		pActor->GetModuleMotion()->PushMotion(strPreMotion.c_str());
		//	}
		//	else	// 커스텀 모션이다
		//		pActor->GetModuleMotion()->PushCustomMotion(strPreMotion.c_str(), strPreAniName.c_str());
		//}
		//else
			pActor->GetModuleMotion()->PushMotion(MOTION_NAME_NPC_IDLE);
	}
	else if (pActor->GetEntityType() == ETID_PLAYER)
	{
		pActor->GetModuleMotion()->PushMotion(MOTION_NAME_IDLE);
	}
}

void XMotionControlCodeProc::_Proc( const string& strMotionName, MUID uidOwner )
{
	XActor* pActor = gg.omgr->FindActor(uidOwner);
	if (pActor == NULL) return;

	//
	pActor->GetModuleMotion()->ChangeMotion(MLocale::ConvAnsiToUTF16(strMotionName.c_str()).c_str());

	if (pActor->GetEntityType() == ETID_NPC)
	{
		pActor->GetModuleMotion()->PushMotion(MOTION_NAME_NPC_IDLE);
	}
	else if (pActor->GetEntityType() == ETID_PLAYER)
	{
		pActor->GetModuleMotion()->PushMotion(MOTION_NAME_IDLE);
	}
}

void XTalentControlCodeProc::_Proc( int nTalentID, MUID uidOwner )
{
	XActor* pActor = gg.omgr->FindActor(uidOwner);
	if (pActor == NULL) return;

	if (pActor->GetModuleTalent())
		pActor->GetModuleTalent()->Start(nTalentID);
}

void XSoundControlCodeProc::_Proc( const string& strSoundName, MUID uidOwner )
{
	XActor* pActor = gg.omgr->FindActor(uidOwner);

	if ( global.sound)
	{
		if ( pActor)
			global.sound->PlayDynamic3D( strSoundName.c_str(), NULL, 0, pActor->GetUID());
		else
			global.sound->Play( strSoundName.c_str());
	}
}


string XNameReplaceCharsProc::_Proc( MUID uidOwner )
{
	if (!gg.omgr) return "";

	XActor* pActor = gg.omgr->FindActor(uidOwner);
	if (pActor == NULL) return "";

	if (pActor->GetEntityType() == ETID_NPC)
	{
		XNonPlayer* pNPC = pActor->AsNPC();
		if (pNPC->GetInfo()) return MLocale::ConvUTF16ToAnsi(pNPC->GetInfo()->GetName());
		else return "";
	}
	else if (pActor->GetEntityType() == ETID_PLAYER)
	{
		XPlayer* pPlayer = static_cast<XPlayer*>(pActor);
		return MLocale::ConvUTF16ToAnsi(pPlayer->GetName());
	}

	return "";
}

string XSexReplaceCharsProc::_Proc( MUID uidOwner )
{
	if (!gg.omgr) return "";

	XActor* pActor = gg.omgr->FindActor(uidOwner);
	if (pActor == NULL) return "";

	if (pActor->GetEntityType() == ETID_PLAYER)
	{
		XPlayer* pPlayer = static_cast<XPlayer*>(pActor);
		return MLocale::ConvUTF16ToAnsi(XStrings::Sex(pPlayer->GetSex()).c_str());
	}

	return "";
}

string XRaceReplaceCharsProc::_Proc( MUID uidOwner )
{
	if (!gg.omgr) return "";

	XActor* pActor = gg.omgr->FindActor(uidOwner);
	if (pActor == NULL) return "";

	if (pActor->GetEntityType() == ETID_PLAYER)
	{
		XPlayer* pPlayer = static_cast<XPlayer*>(pActor);
		return MLocale::ConvUTF16ToAnsi(XStrings::Race(pPlayer->GetRace()).c_str());
	}

	return "";
}