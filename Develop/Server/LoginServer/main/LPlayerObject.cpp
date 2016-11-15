#include "stdafx.h"
#include "LPlayerObject.h"
#include "LGlobal.h"
#include "LTimeoutManager.h"
#include "LCommandCenter.h"


LPlayerObject::LPlayerObject(MUID& uidObject)
: SCommObject(uidObject)
, m_nState(POS_WAITING_FOR_LOGIN)
, m_pConnectingMasterServer(NULL)
, m_nSelectedCharIndex(-1)
{
	;
}

LPlayerObject::~LPlayerObject()
{
	;
}

void LPlayerObject::Create()
{
	SCommObject::Create();
}

void LPlayerObject::Destroy()
{
	SCommObject::Destroy();

	// 감시중의 타임아웃을 중지한다. LTimeoutHandler에서 삭제된 LPlayerObject를 참조하는 경우를 피하기위해.
	gmgr.pTimeoutManager->StopAllWatching(m_UID);
}

void LPlayerObject::InitAccountInfo(ACCOUNT_INFO& info)
{
	m_AccountInfo.nAID = info.nAID;
	m_AccountInfo.bNewAcc = info.bNewAcc;
	_tcsncpy_s(m_AccountInfo.szUserID, info.szUserID, sizeof(info.szUserID) - 1);

	OnSelectChar();
}

void LPlayerObject::AddStatIndex(const PmStatIndex* statIndex, int nPCCafeID)
{
	if (statIndex != NULL)
	{
		m_statIndex = *statIndex;
		m_nPCCafeID = nPCCafeID;
		m_isSetStatIndex = true;
	}
}

PmStatIndex* LPlayerObject::GetStatIndex()
{
	return &m_statIndex;
}

int LPlayerObject::GetPCCafeID()
{
	return m_nPCCafeID;
}

bool LPlayerObject::IsExistStatIndex()
{
	return m_isSetStatIndex;
}

void LPlayerObject::OnSelectChar()
{
	m_nState = POS_SELECT_CHAR;
	m_nSelectedCharIndex = -1;
}

void LPlayerObject::OnLoginGameServer(int nCharIndex)
{
	m_nState = POS_LOGIN_GAME_SERVER;
	m_nSelectedCharIndex = nCharIndex;
}

void LPlayerObject::OnMoveServer()
{
	m_nState = POS_MOVE_SERVER;
}

void LPlayerObject::StartWatchingSelMyCharTimeout()
{
	gmgr.pTimeoutManager->StartWatchingSelMyCharTimeout(m_UID, &m_SelMyCharTimeoutHandler);
}

void LPlayerObject::ClearCharFieldInfo()
{
	m_vecAccountCharFieldInfo.clear();
}

void LPlayerObject::AddCharFieldInfo(const SAccountCharFieldInfo& accountCharFieldInfo)
{
	m_vecAccountCharFieldInfo.push_back(accountCharFieldInfo);
}

void LPlayerObject::RemoveCharFieldInfo(size_t nIndex)
{	
	if (nIndex < m_vecAccountCharFieldInfo.size())
	{
		m_vecAccountCharFieldInfo.erase(m_vecAccountCharFieldInfo.begin() + nIndex);
	}
}

int LPlayerObject::GetCharFieldInfoCount()
{
	return m_vecAccountCharFieldInfo.size();
}

SAccountCharFieldInfo* LPlayerObject::GetCharFieldInfo(size_t nIndex)
{
	if (nIndex < m_vecAccountCharFieldInfo.size())
	{
		return &m_vecAccountCharFieldInfo[nIndex];
	}

	return NULL;
}
