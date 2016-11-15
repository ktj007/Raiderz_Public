#pragma once

#include "GFieldGrid.h"

class GPacketRouteLimiter;

class GPacketRouteLimiterMgr : public MTestMemPool<GPacketRouteLimiterMgr>
{
public:
	GPacketRouteLimiterMgr(GFieldGrid* pGrid);
	~GPacketRouteLimiterMgr(void);
	void ProcessCreate( GFieldGrid::Sector& sector, ENTITY_TYPEID nEntityTypeID );
	void ProcessDestroy( GFieldGrid::Sector& sector, ENTITY_TYPEID nEntityTypeID );
	GPacketRouteLimiter* Get( MPoint ptSector );
	bool TryRoute( const GFieldGrid::Sector& sector, int nCmdID, MUID uidSender );
	int CalcCrowdedLevel( MPoint ptSector, int nPlayerQty );
private:
	typedef map<uint32,GPacketRouteLimiter> PacketLimiterMap;

	GFieldGrid* m_pGrid;
	PacketLimiterMap m_mapPacketLimit;
};
