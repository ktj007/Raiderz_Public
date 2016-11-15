#include "StdAfx.h"
#include "GGuild.h"
#include "GEntityPlayer.h"
#include "GSTLUtil.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GGuildStorage.h"

GGuild::GGuild(int nID, const wchar_t* szName, int nMoney)
: m_nID(nID)
, m_strName(szName)
, m_nMasterCID(0)
, m_nStorageMoney(nMoney)
{
	m_pStorage = new GGuildStorage();
}

GGuild::~GGuild()
{
	SAFE_DELETE(m_pStorage);
	SAFE_DELETE_MAP(m_mapMember);
}

void GGuild::InsertMember(int64 nAID, int nCID, const wchar_t* szName, int nLevel, GUILD_MEMBER_GRADE nGrade)
{
	if (NULL == szName) return;
	_ASSERT(NULL == GetMember(nCID));

	GGuildMember* pNewMember = new GGuildMember(nAID, nCID, szName, nLevel, nGrade);
	m_mapMember.insert(MAP_GUILD_MEMBER::value_type(nCID, pNewMember));

	if (GMG_MASTER == nGrade)
	{
		m_nMasterCID = nCID;
		m_strMasterName = szName;		
	}

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nCID);
	if (NULL != pPlayer)
	{
		pPlayer->GetPlayerInfo()->nGID = m_nID;
	}
}

void GGuild::DeleteMember(int nCID)
{
	MAP_GUILD_MEMBER::iterator itor = m_mapMember.find(nCID);
	if (m_mapMember.end() == itor) return;

	SAFE_DELETE((*itor).second);
	
	m_mapMember.erase(itor);

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nCID);
	if (NULL != pPlayer)
	{
		pPlayer->GetPlayerInfo()->nGID = 0;
	}
}

void GGuild::ChangeGuildMaster(int nOldMasterCID, int nNewMasterCID)
{
	GGuildMember* pOldMasterMember = GetMember(nOldMasterCID);
	if (NULL == pOldMasterMember) return;
	GGuildMember* pNewMasterMember = GetMember(nNewMasterCID);
	if (NULL == pNewMasterMember) return;

	pOldMasterMember->ChangeGrade(GMG_NORMAL);
	pNewMasterMember->ChangeGrade(GMG_MASTER);

	m_nMasterCID = pNewMasterMember->GetCID();
	m_strMasterName = pNewMasterMember->GetName();
}

GGuildMember* GGuild::GetMember(int nCID)
{
	MAP_GUILD_MEMBER::iterator itor = m_mapMember.find(nCID);
	if (m_mapMember.end() == itor) return NULL;
	 
	return (*itor).second;
}

GGuildMember* GGuild::GetMember(const wchar_t* szName)
{
	if (NULL == szName) return NULL;

	for each (const MAP_GUILD_MEMBER::value_type& data in m_mapMember)
	{
		GGuildMember* pMember = data.second;
		if (0 == wcscmp(szName, pMember->GetName()))
		{
			return pMember;
		}
	}

	return NULL;
}

bool GGuild::IsExist(int nCID)
{
	if (NULL == GetMember(nCID)) return false;

	return true;
}

bool GGuild::IsExist(const wchar_t* szName)
{
	if (NULL == GetMember(szName)) return false;

	return true;
}
bool GGuild::IsGuildMaster(int nCID)
{
	return m_nMasterCID == nCID;
}

bool GGuild::IsOnlineMember(int nCID)
{
	GGuildMember* pMember = GetMember(nCID);
	if (NULL == pMember) return false;

	return pMember->IsOnline();
}

vector<int> GGuild::CollectMemberCID()
{
	vector<int> vecMemberCID;

	for each (const MAP_GUILD_MEMBER::value_type& data in m_mapMember)
	{
		GGuildMember* pMember = data.second;
		vecMemberCID.push_back(pMember->GetCID());
	}

	return vecMemberCID;
}

void GGuild::Delete()
{
	for each (const MAP_GUILD_MEMBER::value_type& data in m_mapMember)
	{
		GGuildMember* pMember = data.second;
		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pMember->GetCID());
		if (NULL == pPlayer) continue;

		pPlayer->GetPlayerInfo()->nGID = 0;		
	}
}

void GGuild::DepositStorageMoney( int nMoney )
{
	m_nStorageMoney += nMoney;
}

void GGuild::WithdrawStorageMoney( int nMoney )
{
	m_nStorageMoney -= nMoney;
}

void GGuild::SetStorageMoney( int nMoney )
{
	m_nStorageMoney = nMoney;
}

int GGuild::GetStorageMoney()
{
	return m_nStorageMoney;
}

GGuildStorage* GGuild::GetStorage()
{
	return m_pStorage;
}
