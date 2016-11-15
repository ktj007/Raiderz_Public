#pragma once

#include "GEntityNPC.h"

class GEntityBPart : public GEntityNPC, public MTestMemPool<GEntityBPart>
{
public:
	struct BPartInfo
	{
		int nNPCID;
		int nBPartID;
		int nFlag;

		BPartInfo(): nNPCID(INVALID_ID), nBPartID(INVALID_ID), nFlag(INVALID_ID) {}
	};
public:
	GEntityBPart();
	virtual ~GEntityBPart();

	void SetBPartInfo(int nNPCID, int nBPartID, int nFlag);

	virtual void RouteInEntity(const vector<MUID>& vecPlayers) override;
	virtual void RouteOutEntity( const vector<MUID>& vecPlayers ) override;
	void MakeTDCacheInfo(GEntityPlayer* pReqPlayer, TD_UPDATE_CACHE_PBART& out );

protected:
	virtual MCommand* CreateFieldInCmd() override;
	virtual MCommand* CreateFieldOutCmd() override;

private:
	BPartInfo m_infoBPart;
};