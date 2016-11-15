#include "stdafx.h"
#include "XGlueGamePartyFunction.h"
#include "XCommonFunction.h"
#include "XInteractionInfo.h"
#include "XPost_Action.h"
#include "XPost_Msg.h"

//파티초대
void XGlueGamePartyFunction::PartyInviteReq()
{
	if (XGetInteractionInfo().InteractionTargetUID ==  gvar.MyInfo.MyUID)
	{
		global.ui->OnSystemMsg(L"자기 자신에게 파티를 신청할 수 없습니다.");
	}
	else
	{
		XPostPartyRequestInvite(XGetInteractionInfo().InteractionTargetUID);
	
		wstring strPlayerName = XCommonFunction::GetPlayerName(XGetInteractionInfo().GetInteractionSearchUID());
		global.ui->OnSystemMsg(L"{0}님께 파티를 신청 하였습니다.", FSParam(strPlayerName));
	}
}

void XGlueGamePartyFunction::PartyRequiredAnswer( bool bAccept )
{
	//XPostPartyRequestInviteQuestionRespond(bAccept);

	//char text[1024]="";
	//sprintf_s(text, "파티초대를 수락하지 않았습니다.");
}

void XGlueGamePartyFunction::PartyConfirmRequire()
{
	//파티가입요청
}

void XGlueGamePartyFunction::PartyLeave()
{
	//우선 자기가 탈퇴하는 걸로.
	XPostPartyRequestLeave();
}

void XGlueGamePartyFunction::PartyFired(const char* szuid)
{
	if( szuid == NULL) return;

	MUID uid = _atoi64(szuid);
	XPostPartyRequestKick(uid);
}

void XGlueGamePartyFunction::PartyDivisionRule()
{

}

void XGlueGamePartyFunction::PartyMessage()
{
	wchar_t* pszText = {L"아직 멀었습니다.작업중"};
	XPostMsg_Party(pszText);
}

//파티요청
void XGlueGamePartyFunction::PartyDemand()
{
	//파티장 uid가 아니라 파티의 uid를 날려야 한다.
	// 인터페이스에서 파티목록을 구현하고 그목록중 파티를 선택해서
	// 파티uid를 날린다.
	//XPostPartyRequestInviteForMe();
}

void XGlueGamePartyFunction::PartyDemandAnswer(bool bAccept)
{
	XPostPartyRequestInviteForMeQuestionRespond(bAccept);
}

__int64 XGlueGamePartyFunction::GetPartyLeaderUID()
{
	return gvar.MyInfo.Party.GetLeader().Value;
}

bool XGlueGamePartyFunction::IsPartyMemberOffline(const char* szuid)
{
	if( szuid == NULL) return NULL;

	MUID uid = _atoi64(szuid);
	return gvar.MyInfo.Party.IsPartyMemberOffline(uid);
}

bool XGlueGamePartyFunction::IsPartyMemberDead(const char* szuid)
{
	if( szuid == NULL) return NULL;

	MUID uid = _atoi64(szuid);
	return gvar.MyInfo.Party.IsPartyMemberDead(uid);
}

const char* XGlueGamePartyFunction::GetPartyMemberName(const char* szuid)
{
	if( szuid == NULL) return NULL;

	MUID uid = _atoi64(szuid);
	
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(gvar.MyInfo.Party.GetPartyMemberName(uid));

	return gvar.Temp.szTemp.c_str();
}

int XGlueGamePartyFunction::GetPartyMemberHP( const char* szuid )
{
	if( szuid == NULL) return NULL;

	MUID uid = _atoi64(szuid);
	return gvar.MyInfo.Party.GetPartyMemberHP(uid);
}

int XGlueGamePartyFunction::GetPartyMemberEN( const char* szuid )
{
	if( szuid == NULL) return NULL;

	MUID uid = _atoi64(szuid);
	return gvar.MyInfo.Party.GetPartyMemberEN(uid);
}

int XGlueGamePartyFunction::GetPartyMemberSTA( const char* szuid )
{
	if( szuid == NULL) return NULL;

	MUID uid = _atoi64(szuid);
	return gvar.MyInfo.Party.GetPartyMemberSTA(uid);
}

const char* XGlueGamePartyFunction::GetPartyMemberLevel(const char* szuid)
{
	if( szuid == NULL) return NULL;

	MUID uid = _atoi64(szuid);
	return "None";//gvar.MyInfo.Party.GetPartyMemberLevel(uid);
}

const char* XGlueGamePartyFunction::GetPartyMemberClass(const char* szuid)
{
	if( szuid == NULL) return NULL;

	MUID uid = _atoi64(szuid);
	return "None";//gvar.MyInfo.Party.GetPartyMemberLevel(uid);
}

bool XGlueGamePartyFunction::IsPartyMemberHightlight( const char* szuid )
{
	if( szuid == NULL) return NULL;

	MUID uid = _atoi64(szuid);
	return gvar.MyInfo.Party.IsPartyMemberHightlight(uid);
}



