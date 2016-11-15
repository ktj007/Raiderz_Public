#ifndef _GENTITY_MGR_H
#define _GENTITY_MGR_H

#include "MUID.h"
#include "MTypes.h"
#include "MMemPool.h"
#include "GEntity.h"
#include "GIndexedIDMap.h"
#include <vector>
using namespace std;


class GField;
class GEntityPlayer;
class GEntityNPC;
class GEntityActor;
class GBuffEntity;
class GNPCInfo;

class GGatherEntityCounter
{
private:
	int	m_nCount[MAX_GATHER_TYPE];

	void _Inc(GATHER_TYPE nGatherType);
	void _Dec(GATHER_TYPE nGatherType);
public:
	GGatherEntityCounter();
	~GGatherEntityCounter() {}
	void Inc(GNPCInfo* pNPCInfo);
	void Dec(GNPCInfo* pNPCInfo);
	int GetCount(GATHER_TYPE nGatherType)
	{
		return m_nCount[nGatherType];
	}
};

/// 엔티티 관리자
class GEntityMgr : public MMemPool<GEntityMgr>
{
public:
	typedef tr1::unordered_map<uint64, GEntity*>	ENTITY_UID_MAP;
	typedef map<uint64, GEntityNPC*>		NPC_UID_MAP;
	typedef map<int,	NPC_UID_MAP>		NPC_ID_MAP;
private:
	ENTITY_UID_MAP		m_EntityUIDMap;
	NPC_ID_MAP			m_NPCIDMap;
	map<int, MUID>		m_StaticEntityMap;
	int					m_nEntityQty[ETID_MAX];
	GGatherEntityCounter	m_GatherEntityCounter;

	// 업데이트용
	vector<GEntity*>	m_vecEntitiesExceptPlayer;
	vector<GEntity*>	m_vecPlayers;

	// Index ID
	GIndexedNPCUIDMap	m_IndexedNPCUIDFinder;		///< NPC Index ID 매니저

	vector<GEntity*>		m_vecDeleteEntity;
private:
	void AddEntity_UID(GEntity* pEntity);
	void AddNPC_ID(GEntityNPC* pNPC);
	void AddNPC_StaticID(GEntityNPC* pNPC);
	void AddNPC_UIID(GEntityNPC* pNPC);
	void AddNPC_GatherEntityCount(GEntityNPC* pNPC);

	void RemoveEntity_UID(GEntity* pEntity);
	void RemoveNPC_ID(GEntityNPC* pNPC);
	void RemoveNPC_StaticID(GEntityNPC* pNPC);	
	void RemoveNPC_UIID(GEntityNPC* pNPC);
	void RemoveNPC_GatherEntityCount(GEntityNPC* pNPC);

	void DestoryEntity(GEntity* pEntity);

protected:
	void UpdateEntity_Players(float fDelta);
	bool UpdateEntityImpl(GEntity* pEntity, float fDelta);
	void UpdateEntity_ExceptPlayers(float fDelta);
public:
	GEntityMgr();
	~GEntityMgr();
	void Create();
	void Destroy();
	void Update(float fDelta);
	void Clear(bool bExceptPlayers=false);
	void AddEntity(GEntity* pEntity);
	void RemoveEntity(GEntity* pEntity, bool bDelete);
	GEntity*		FindEntity(const MUID& uidEntity);
	GEntity*		FindEntityFromStaticID(int nStaticID);
	GEntityPlayer*	FindPlayer(const MUID& uidPlayer);
	GEntityNPC*		FindNPC(const MUID& uidNPC);
	GEntityNPC*		FindNPC(const UIID nNPCUIID);
	vector<GEntityNPC*>	FindNPCByID(int nNPCID);
	GEntityNPC*		FindOneNPCByID(int nNPCID);
	GEntityActor*	FindActor(const MUID& uidActor);
	GBuffEntity*	FindBuffEntity(const MUID& uidMagicArea);
	void			GetNeighborNPC(const vec3& vPos, const float fRange, vector<GEntityNPC*>& vecOutNeighborNPC);
	int				GetEntityQty(ENTITY_TYPEID nType);
	int				GetLiveNPCCount(int nNPCID);
	vector<GEntityPlayer*>	GetPlayers();
	vector<GEntityNPC*>		GetNPCs();
	vector<GEntityActor*> GetActors();
	void GetPlayerUIDs(vector<MUID>& vecPlayerUIDs);

	void			ActiveSpawnGroup(int nGroupID);

	// 채집
	int				GetGatherEntityCount(GATHER_TYPE nGatherType);

	ENTITY_UID_MAP& GetEntityMap()		{ return m_EntityUIDMap; }	
};


#endif
