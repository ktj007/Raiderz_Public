#pragma once

#include "MPathCollision.h"
#include "MTime.h"

class GEntityNPC;
class GEntity;
struct SPathMesh;

class GPathContextGroup
{
private:
	SPathMesh*					m_pPathMesh;
	map<MUID, MContext*>		m_ContextMap;
	MRegulator					m_Updater;

	MContext* InsertContext(MUID& uidTarget);
	void DeleteContext(MContext* pContext);
public:
	GPathContextGroup();
	~GPathContextGroup();
	void Init(int nZoneID);
	void Register(MUID& uidTarget, GEntityNPC* pNPC);
	void Revoke(GEntityNPC* pNPC);
	void OnRemoveEntity(GEntity* pEntity);
	void Update(float fDelta);

	// for test
	size_t GetContextCount() { return m_ContextMap.size(); }
};
