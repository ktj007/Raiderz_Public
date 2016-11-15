#include "stdafx.h"
#include "ZPlayer.h"
#include "ZFieldManager.h"
#include "ZDynamicField.h"
#include "ZGameServerObjectManager.h"


ZPlayer::ZPlayer(MUID uidPlayer, AID nAID, const std::wstring& strUserID)
: m_UID(uidPlayer)
, m_AID(nAID)
, m_strUserID(strUserID)
, m_CID(INVALID_ID)
, m_nGameServerID(INVALID_ID)
, m_uidField(MUID::ZERO)
, m_uidParty(MUID::ZERO)
, m_nGuildID(INVALID_ID)
, m_eState(PS_SELECT_CHAR)
, m_nFieldID(INVALID_ID)
, m_nChannelID(INVALID_ID)
, m_nReservedGameServerID(INVALID_ID)
, m_uidReservedField(MUID::ZERO)
{}

void ZPlayer::SetWorldInfo(CID nCID, int nGameServerID, MUID uidPlayer, const std::wstring& strPlayerName, int nGID)
{
	m_nReservedGameServerID = INVALID_ID;

	m_CID = nCID;
	m_nGameServerID = nGameServerID;
	m_UID = uidPlayer;
	m_strPlayerName = strPlayerName;
	m_nGuildID = nGID;
}

void ZPlayer::ClearWorldInfo()
{
	m_CID = INVALID_ID;
	m_nGameServerID = INVALID_ID;
	m_nReservedGameServerID = INVALID_ID;
	m_UID.SetInvalid();
	m_strPlayerName.clear();
	m_nGuildID = INVALID_ID;
}

void ZPlayer::SetUID(MUID uidPlayer)
{
	m_UID = uidPlayer;
}

void ZPlayer::SetFieldUID( MUID uidField )
{
	m_uidField = uidField;
}

void ZPlayer::SetPartyUID(MUID uidParty)
{
	m_uidParty = uidParty;
}

void ZPlayer::SetGuildID(int nGuildID)
{
	m_nGuildID = nGuildID;
}

void ZPlayer::SetFieldID(int nFieldID)
{
	m_nFieldID = nFieldID;
}

void ZPlayer::SetChannelID( int nChannelID )
{
	m_nChannelID = nChannelID;
}

MUID ZPlayer::GetFieldGroupUID() const
{
	ZDynamicField* pDynamicField = ToDynamicField(gmgr.pFieldManager->FindField(m_uidField));
	if (NULL == pDynamicField) return MUID::Invalid();
	return pDynamicField->GetFieldGroupUID();
}

void ZPlayer::AddPmangUserData(PmUserData* pPmangUserData)
{
	if (pPmangUserData != NULL)
	{
		m_pmangUserData = *pPmangUserData;
	}
}

PmUserData* ZPlayer::GetUserPmangData()
{
	return &m_pmangUserData;
}

bool ZPlayer::IsExistPmangUserData() const
{
	return !m_pmangUserData.strUserSerial.empty();
}

MUID ZPlayer::GetGameServerUID()
{
	return gmgr.pGameServerObjectManager->IDToUID(m_nGameServerID);
}