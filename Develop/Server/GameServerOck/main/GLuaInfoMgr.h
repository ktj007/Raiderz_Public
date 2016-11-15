#pragma once

class GLuaInfoMgr
{
	// Calling Lua 
	// 루아에서 사용할 id는 각 NPC별로 따로 생성한다
	// C++에선 NPC객체가 Field 단위로 분리되어 생성되지만 루아에선 전역적으로 생성되므로
	// id를 필드 전역적으로 만든다
typedef map<int, int>			ID_MANAGER_MAP;
typedef map<int, vector<int>>	ID_CONTAINER_MAP;
typedef map<int, MUID>			ID_PAIR_MAP;
typedef map<int, ID_PAIR_MAP>	ID_PAIR_CONTAINER_MAP;

	ID_MANAGER_MAP				m_mapNPCIDMgr;				// <npcid, lua id 갯수>
	ID_CONTAINER_MAP			m_mapNPCIDContainer;		// <npcid, npcid에 해당하는 npc들의 id vector>
	ID_PAIR_CONTAINER_MAP		m_mapNPCIDPairContainer;	// <npcid, luaid-muid map>


public:
	GLuaInfoMgr(void);
	~GLuaInfoMgr(void);

	static GLuaInfoMgr& GetInstance();

	void	InitNPC(int nNPCId);
	int		AddNPC(int nNPCId, const MUID uid);
	void	RemoveNPC(int nNPCId, int nLuaId);

	MUID	GetNPCUID(int nNPCID, int nLuaID);
};
