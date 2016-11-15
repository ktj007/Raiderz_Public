#include "StdAfx.h"
#include "GLuaInfoMgr.h"
#include <algorithm>

GLuaInfoMgr::GLuaInfoMgr(void)
{
}

GLuaInfoMgr::~GLuaInfoMgr(void)
{
}

GLuaInfoMgr& GLuaInfoMgr::GetInstance()
{
	static GLuaInfoMgr m_Instance;
	return m_Instance;
}

// 해당 NPC Map에 id를 추가
void GLuaInfoMgr::InitNPC(int nNPCId)
{
	m_mapNPCIDMgr.insert(ID_MANAGER_MAP::value_type(nNPCId, 1));	// 초기값으로 1
	m_mapNPCIDContainer.insert(ID_CONTAINER_MAP::value_type(nNPCId, vector<int>()));
	m_mapNPCIDPairContainer.insert(ID_PAIR_CONTAINER_MAP::value_type(nNPCId, ID_PAIR_MAP()));
}

// 해당 NPC의 lua id 등록
int GLuaInfoMgr::AddNPC(int nNPCId, const MUID uid)
{
	assert(m_mapNPCIDMgr.find(nNPCId) != m_mapNPCIDMgr.end());
	assert(m_mapNPCIDContainer.find(nNPCId) != m_mapNPCIDContainer.end());
	
	int nLuaID = m_mapNPCIDMgr[nNPCId]++;
	m_mapNPCIDContainer[nNPCId].push_back(nLuaID);

	m_mapNPCIDPairContainer[nNPCId].insert(ID_PAIR_MAP::value_type(nLuaID, uid));

	return nLuaID;
}

// 해당 NPC의 lua id 삭제
void GLuaInfoMgr::RemoveNPC(int nNPCId, int nLuaId)
{
	assert(m_mapNPCIDMgr.find(nNPCId) != m_mapNPCIDMgr.end());
	assert(m_mapNPCIDContainer.find(nNPCId) != m_mapNPCIDContainer.end());

	vector<int>::iterator itr = find(m_mapNPCIDContainer[nNPCId].begin(), m_mapNPCIDContainer[nNPCId].end(), nLuaId);

	if(itr != m_mapNPCIDContainer[nNPCId].end())
		m_mapNPCIDContainer[nNPCId].erase(itr);
}

MUID GLuaInfoMgr::GetNPCUID(int nNPCID, int nLuaID)
{
	assert(m_mapNPCIDPairContainer.find(nNPCID) != m_mapNPCIDPairContainer.end());
	assert(m_mapNPCIDPairContainer[nNPCID].find(nLuaID) != m_mapNPCIDPairContainer[nNPCID].end());

	return m_mapNPCIDPairContainer[nNPCID].find(nLuaID)->second;
}