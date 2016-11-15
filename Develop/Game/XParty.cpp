#include "stdafx.h"
#include "XParty.h"
#include "XPost_Comm.h"

bool XPartyMember::IsDead()
{
	return (CheckBitSet(nStatusFlag, UPS_DEAD) > 0);
}

bool XPartyMember::IsOffline()
{
	return (CheckBitSet(nStatusFlag, UPS_OFFLINE) > 0);
}

bool XPartyMember::IsHightLight()
{
	return (CheckBitSet(nEffectFlag, ME_HIGHTLIGHT) > 0);
}

XParty::XParty()
: nQuestIDWithCurrentSensor(0)
{
	SetPartyState(0, AUTOPARTY_STANDBY);
	Clear();
}

XParty::~XParty()
{
}

void XParty::Clear()
{
	PartySetting.Init();
	clear();
}

void XParty::Init(const TD_PARTY& tdPartyInfo)
{
	Clear();	
	uidParty	= tdPartyInfo.m_uidParty;

	tdPartyInfo.m_partySetting.Export(PartySetting);
}

void XParty::AddMember(const TD_PARTY_MEMBER& tdMember)
{
	XPartyMember Member;
	SetMember(Member, tdMember);

	insert(PARTY_SET::value_type(Member.m_uid, Member));
}

void XParty::DeleteMember(const MUID& uidMember)
{
	erase(uidMember);
}

bool XParty::Find( const MUID& uidPlayer )
{
	iterator iter = find(uidPlayer);

	if (iter == End())
		return false;

	return true;
}

bool XParty::Find( const MUID& uidPlayer, XPartyMember& outMember )
{
	iterator iter = find(uidPlayer);

	if (iter == End())
		return false;

	outMember = iter->second;

	return true;
}

void XParty::ChangeStatus( const TD_PARTY_MEMBER& tdMember )
{
	XPartyMember Member;
	iterator iter = find(tdMember.m_uidPlayer);
	if (iter == End())	return;

	SetMember(Member, tdMember);

	iter->second = Member;
}

void XParty::SetMember( XPartyMember& Member, const TD_PARTY_MEMBER& tdMember )
{
	Member.m_uid		= tdMember.m_uidPlayer;
	Member.m_strName	= tdMember.szName;
	Member.nStatusFlag  = tdMember.nStatusFlag;
	Member.nHP			= tdMember.nHP;
	Member.nEN			= tdMember.nEN;
	Member.nSTA			= tdMember.nSTA;
	Member.nLevel		= tdMember.nLevel;
}

bool XParty::IsPartyMemberDead(const MUID& uidPlayer)
{
	iterator iter = find(uidPlayer);

	if (iter == End())
		return false;

	return iter->second.IsDead();
}

bool XParty::IsPartyMemberOffline(const MUID& uidPlayer)
{
	iterator iter = find(uidPlayer);

	if (iter == End())
		return false;

	return iter->second.IsOffline();
}

bool XParty::IsPartyMember( const MUID& uidPlayer )
{
	iterator iter = find(uidPlayer);

	if (iter == End())
		return false;

	return true;
}

//const char* XParty::GetMemberUIDList(wstring& szText)
//{
//	char buff[512] = {0,};
//	szText = "";
//	for( iterator it = begin(); it != end(); ++it)
//	{
//		if( it == begin())
//			sprintf_s(buff, "\"%I64d\"", it->second.m_uid.Value);
//		else
//			sprintf_s(buff, ",\"%I64d\"", it->second.m_uid.Value);
//
//		szText += buff;
//	}
//
//	return szText.c_str();
//}

bool XParty::GetIndex( size_t nIndex, XPartyMember** ppOutMember)
{
	// 비어있거나, 인덱스 초과 확인
	if (size() <= nIndex)	return false;
	
	// 인덱스번째 값 찾아가기
	iterator it = begin();
	for(size_t i = 0; i < nIndex; i++)
	{
		it++;
	}

	// 해당 파티원 반환
	*ppOutMember = &(*it).second;
	return true;
}

const wchar_t* XParty::GetPartyMemberName(const MUID& uidPlayer)
{
	iterator iter = find(uidPlayer);
	if (iter == End())	return L"";

	return iter->second.m_strName.c_str();
}

int XParty::GetPartyMemberHP( const MUID& uidPlayer )
{
	iterator iter = find(uidPlayer);
	if (iter == End())	return 0;

	return iter->second.nHP;
}

int XParty::GetPartyMemberEN( const MUID& uidPlayer )
{
	iterator iter = find(uidPlayer);
	if (iter == End())	return 0;

	return iter->second.nEN;

}

int XParty::GetPartyMemberSTA( const MUID& uidPlayer )
{
	iterator iter = find(uidPlayer);
	if (iter == End())	return 0;

	return iter->second.nSTA;

}

int XParty::GetPartyMemberLevel(const MUID& uidPlayer)
{
	iterator iter = find(uidPlayer);
	if (iter == End())	return 0;

	return iter->second.nLevel;
}

bool XParty::IsJoined()
{
	return (!empty());
}

bool XParty::SetPartyMemberBuffID(const MUID& uidPlayer, int nBuffID)
{
	iterator iter = find(uidPlayer);

	if (iter == End())
		return false;

	iter->second.nBuffIDList.push_back(nBuffID);

	return true;
}

//const char* XParty::GetPartyMemberBuffIDList(const MUID& uidPlayer, wstring& szText)
//{
//	iterator it = find(uidPlayer);
//
//	if (it == End())
//		return "";
//
//	char buff[512] = {0,};
//	szText = "";
//
//	int count = it->second.nBuffIDList.size();
//	for (int i = 0; i < count; ++i)
//	{
//		if ( i != 0 )
//		{
//			szText += ",";
//		}
//		sprintf_s(buff, "%d", it->second.nBuffIDList[i]);
//		szText += buff;
//	}
//
//	return szText.c_str();
//}

bool XParty::IsPartyMemberReconnect(const TD_PARTY_MEMBER& tdMember)
{
	if (CheckBitSet(tdMember.nStatusFlag, UPS_OFFLINE))
	{
		return false;
	}

	for( iterator it = begin(); it != end(); ++it)
	{
		XPartyMember& partymember = it->second;
		if (partymember.IsOffline() &&
			!wcscmp(tdMember.szName, partymember.m_strName.c_str()))
		{
			return true;
		}
	}

	return false;
}

//void XParty::ChangeName( wstring strName )
//{
//	wcsncpy_s(PartySetting.m_szName, strName.c_str(), PARTY_NAME_LEN);
//}

void XParty::ChangeLeader( MUID uidLeader )
{
	PartySetting.m_uidLeader = uidLeader;
}

void XParty::ChangeLootingRule( LOOTING_RULE_DATA rule )
{
	PartySetting.m_lootingRuleData = rule;
}

void XParty::SetPartyState( int nQuestID, int nAutoPartyState )
{
	if (nQuestID != 0)
		PartySetting.m_nAutoPartyQuestID = nQuestID;

	m_nAutoPartyState = (AUTOPARTY_STATE)nAutoPartyState;

	//switch (m_nAutoPartyState)
	//{
	//case 0:	{ mlog("AUTOPARTY_OFF\n");		} break;
	//case 1:	{ mlog("AUTOPARTY_PAUSE\n");	} break;
	//case 2:	{ mlog("AUTOPARTY_STANDBY\n");	} break;
	//case 3:	{ mlog("AUTOPARTY_LOOKUP\n");	} break;
	//}
}

bool XParty::IsLeader()
{
	return GetLeader() == XGetMyUID();
}

// 센서의 퀘스트 아이디 파티의 퀘스트 아이디 현재 받은 퀘스트아이디
void XParty::QuestGive(int nQuestID)
{
	if (nQuestIDWithCurrentSensor == 0 ||
		nQuestIDWithCurrentSensor != nQuestID) return;

	if (GetPartyState() == AUTOPARTY_STANDBY) return;

	SetPartyState(nQuestID, AUTOPARTY_STANDBY);

	if (IsJoined() == true && IsLeader() == true)
	{
		XPostRequestAutoPartyChangeState(nQuestID, AUTOPARTY_STANDBY);
	}
}

void XParty::QuestCancel(int nQuestID)
{
	if (nQuestIDWithCurrentSensor == 0 ||
		nQuestIDWithCurrentSensor != nQuestID) return;

	if (GetPartyState() == AUTOPARTY_PAUSE) return;

	SetPartyState(nQuestID, AUTOPARTY_PAUSE);

	if (IsJoined() == true && IsLeader() == true)
	{
		XPostRequestAutoPartyChangeState(nQuestID, AUTOPARTY_PAUSE);
	}
}

void XParty::ChangeAutoPartyState( int nQuestID, int nAutoPartyState )
{
	if (GetPartyState() != nAutoPartyState)
	{
		SetPartyState(nQuestID, nAutoPartyState);

		if (IsJoined() == true && IsLeader() == false)
		{
			wstring strMsg = CSFormatString::Format(L"파티장이 파티상태를 {0:사용안함으로;정지로;검색중으로} 변경하셨습니다.", FSParam(nAutoPartyState));
			global.ui->OnSystemMsg(strMsg.c_str());
		}
	}
}

bool XParty::IsPartyMemberHightlight( const MUID& uidPlayer )
{
	iterator iter = find(uidPlayer);

	if (iter == End())
		return false;

	return iter->second.IsHightLight();
}

void XParty::SetPartyMemberEffect( const MUID& uidPlayer, MEMBER_EFFECT eEffect )
{
	iterator iter = find(uidPlayer);

	if (iter == End())
		return;

	if(eEffect == ME_NONE)
	{
		ClearBitSet(iter->second.nEffectFlag, ME_HIGHTLIGHT);
		return;
	}

	SetBitSet(iter->second.nEffectFlag, eEffect);
}

void XParty::ClearPartyMemberEffect( MEMBER_EFFECT eEffect )
{
	for(PARTY_SET::iterator it = begin(); it != end(); ++it)
	{
		ClearBitSet(it->second.nEffectFlag, eEffect);
	}
}
