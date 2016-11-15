#include "stdafx.h"
#include "SProxyPlayerManager.h"
#include "SProxyPlayer.h"
#include "SCmdHandler_ProxyPlayer.h"
#include "STransData.h"

SProxyPlayerManager::SProxyPlayerManager(minet::MCommandCommunicator* pCC)
{		
	m_pCmdHandler = new SCmdHandler_ProxyPlayer(pCC, this);
}

SProxyPlayerManager::~SProxyPlayerManager()
{
	SAFE_DELETE(m_pCmdHandler);
	Clear();	
}

bool SProxyPlayerManager::AddPlayer(const TD_PROXY_PLAYER_INFO& info)
{	
	CID cid = info.nCID;	

	player_map::iterator it = m_mapPlayer.find(cid);
	if (it != m_mapPlayer.end())	return false;

	OnPreAdd(cid);

	SProxyPlayer* pPlayer = new SProxyPlayer(cid);	
	pPlayer->Update(info);
	m_mapPlayer.insert(make_pair(cid, pPlayer));
	
	m_mapName.insert(make_pair(info.szName, pPlayer));

	OnAddComplate(pPlayer);

	return true;
}

bool SProxyPlayerManager::RemovePlayer(CID cid)
{	
	// 플레이어 찾기
	player_map::iterator itPlayer = m_mapPlayer.find(cid);
	if (itPlayer == m_mapPlayer.end())	return false;

	SProxyPlayer* pPlayer = itPlayer->second;


	// 이름 목록에서도 찾기
	wstring strName = pPlayer->GetName();
	name_map::iterator itName = m_mapName.find(strName);
	if (itName == m_mapName.end())		return false;

	
	// 제거
	OnPreRemove(pPlayer);
	
	SAFE_DELETE(pPlayer);
	m_mapPlayer.erase(itPlayer);
	m_mapName.erase(itName);

	OnRemoveComplate(cid);

	return true;
}

bool SProxyPlayerManager::UpdatePlayer(const TD_PROXY_PLAYER_INFO& info)
{
	// 플레이어 찾기
	player_map::iterator itPlayer = m_mapPlayer.find(info.nCID);
	if (itPlayer == m_mapPlayer.end())	return false;
	
	SProxyPlayer* pPlayer = itPlayer->second;


	// 이름 목록에서도 찾기
	wstring strName = pPlayer->GetName();
	name_map::iterator itName = m_mapName.find(strName);
	if (itName == m_mapName.end())		return false;

	
	// 갱신	
	OnPreUpdate(pPlayer, info);
		
	// 이름 변경 시, 이름 목록 새로 만들기
	if (strName != info.szName)
	{
		m_mapName.erase(itName);		
		m_mapName.insert(make_pair(info.szName, pPlayer));
	}

	pPlayer->Update(info);	
	
	OnUpdateComplate(pPlayer);

	return true;
}

void SProxyPlayerManager::Clear(void)
{
	for (player_map::iterator it = m_mapPlayer.begin(); it != m_mapPlayer.end(); it++)
	{
		CID cid = it->first;
		SProxyPlayer* pPlayer = it->second;

		OnPreRemove(pPlayer);
		SAFE_DELETE(pPlayer);
		OnRemoveComplate(cid);
	}
	m_mapPlayer.clear();
	m_mapName.clear();
}

SProxyPlayer* SProxyPlayerManager::FindPlayer(CID cid)
{
	player_map::iterator it = m_mapPlayer.find(cid);
	if (it == m_mapPlayer.end())	return NULL;

	return it->second;
}

SProxyPlayer* SProxyPlayerManager::FindPlayer(wstring strName)
{
	name_map::iterator it = m_mapName.find(strName);
	if (it == m_mapName.end())	return NULL;

	return it->second;
}

int	SProxyPlayerManager::GetPlayerCount(void) const
{
	return m_mapPlayer.size();
}
