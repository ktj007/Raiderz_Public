#pragma once

#include "CSEmblemInfoMgr.h"

TEST_FORWARD_F(Emblem, FEmblem);

class GEmblemInfoMgr : public CSEmblemInfoMgr, public MTestMemPool<GEmblemInfoMgr>
{
	TEST_FRIEND_F(Emblem, FEmblem);
public:
	GEmblemInfoMgr(void);
	~GEmblemInfoMgr(void);

	int GetKillNPCIDByEmblem(int nEmblemID);
	int GetKillNPCIDByCondition(int nConditionID);
	vector<const CSEmblemInfo*> GetKillEmblem(int nNPCID);

private:
	virtual void OnLoadCompleted() override;
private:
	typedef map<int,vector<const CSEmblemInfo*>> KillEmblemMap;
	KillEmblemMap m_mapKillEmblems;
};
