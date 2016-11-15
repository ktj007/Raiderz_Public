#pragma once

#include "GEntitySync.h"
#include "CTransData.h"

class GModuleBuff;

class GBuffEntity : public GEntitySync, public MTestMemPool<GBuffEntity>
{
public:
	GBuffEntity(GField* pField, vec3 vPoint, MUID uidOwner, int nIncludeBuffID);
	~GBuffEntity(void);

	static GBuffEntity* Spawn( GEntityActor* pOwner, vec3 vPoint, int nIncludeBuffID, int nLimitQty=0 );
	virtual bool Create(MUID& uid);
	virtual void OnUpdate(float fDelta) override;

	bool HasBuff( int nBuffID );
	int GetBuffID() const;

	GEntityActor* GetOwner();

	GModuleBuff* GetModuleBuff() const { return m_pModuleBuff; }
	
	void MakeTDCacheInfo(TD_UPDATE_CACHE_BUFFENTITY& out);
private:
	virtual void OnDestroy() override;
	virtual void RouteInEntity(const vector<MUID>& vecPlayers) override;
	virtual void RouteOutEntity(const vector<MUID>& vecPlayers) override;

public:
	virtual MCommand* CreateFieldInCmd() override;
	virtual MCommand* CreateFieldOutCmd() override;

private:
	MUID					m_uidOwner;
	GModuleBuff*			m_pModuleBuff;
	// 해당 버프가 존재해야 유지됨
	int m_nIncludeBuffID;
};
