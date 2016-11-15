#include "StdAfx.h"
#include "GGuildMgr.h"
#include "GGuild.h"
#include "GEntityPlayer.h"
#include "GSTLUtil.h"

GGuildMgr::GGuildMgr()
{
}

GGuildMgr::~GGuildMgr()
{
	Clear();
}

void GGuildMgr::Clear()
{
	SAFE_DELETE_MAP(m_mapGuild);
	m_mapGuildInvite.clear();
}

GGuild* GGuildMgr::Insert(int nID, const wchar_t* szName, int nMoney)
{
	GGuild* pNewGuild = new GGuild(nID, szName, nMoney);
	m_mapGuild.insert(MAP_GUILD::value_type(pNewGuild->GetID(), pNewGuild));
	m_mapGuildByName.insert(MAP_GUILD_BY_NAME::value_type(pNewGuild->GetName(), pNewGuild));
	return pNewGuild;
}

void GGuildMgr::Delete(int nID)
{
	GGuild* pGuild = Get(nID);
	if (NULL == pGuild) return;

	pGuild->Delete();

	m_mapGuild.erase(nID);
	m_mapGuildByName.erase(pGuild->GetName());
	SAFE_DELETE(pGuild);	
}

GGuild* GGuildMgr::Get(int nID)
{
	MAP_GUILD::iterator itor = m_mapGuild.find(nID);
	if (m_mapGuild.end() == itor) return NULL;

	return (*itor).second;
}

GGuildStorage* GGuildMgr::GetStorage( int nID )
{
	GGuild* pGuild = Get(nID);
	if (NULL == pGuild) return NULL;

	return pGuild->GetStorage();
}

const wchar_t* GGuildMgr::GetName(int nID)
{
	GGuild* pGuild = Get(nID);
	if (NULL == pGuild) return NULL;

	return pGuild->GetName();
}

bool GGuildMgr::IsExist(int nID)
{
	GGuild* pGuild = Get(nID);
	if (NULL == pGuild) return false;

	return true;
}

bool GGuildMgr::IsExist(const wchar_t* szName)
{
	VALID_RET(NULL != szName, false);

	MAP_GUILD_BY_NAME::iterator itor = m_mapGuildByName.find(szName);
	if (itor == m_mapGuildByName.end())
	{
		return false;
	}
	return true;
}

void GGuildMgr::Invite(CID nGuestCID, const CID nInvitorCID)
{
	MAP_GUILD_INVITE::iterator itor = m_mapGuildInvite.find(nGuestCID);
	if (m_mapGuildInvite.end() == itor)
	{
		m_mapGuildInvite.insert(MAP_GUILD_INVITE::value_type(nGuestCID, nInvitorCID));
	}
	else
	{
		itor->second = nInvitorCID;
	}	
}

void GGuildMgr::InviteEnd(CID nGuestCID)
{
	m_mapGuildInvite.erase(nGuestCID);
}

CID GGuildMgr::GetInvitorCID(CID nGuestCID)
{
	MAP_GUILD_INVITE::iterator itor = m_mapGuildInvite.find(nGuestCID);
	if (m_mapGuildInvite.end() == itor) return NULL;

	CID nInvitorCID = (*itor).second;

	return nInvitorCID;	
}
