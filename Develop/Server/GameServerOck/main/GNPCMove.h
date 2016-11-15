#pragma once

#include "CTransData.h"

class GEntityNPC;

class GNPCMove : public MTestMemPool<GNPCMove>
{
public:
	GNPCMove(GEntityNPC* pOwner);
	~GNPCMove(void);
	
	void MakeTD(TD_UPDATE_CACHE_NPC& out) const;

	float GetSpeed() const;
	float GetRunSpeed() const;
	float GetFleeSpeed() const;

	void SetAlwaysRun(bool bRun);
	void SetRetreat(bool var);
	void SetReturnToMove(bool var);
	bool IsRun() const;
	bool IsRetreat() const { return m_bNowRetreat; }
	bool IsReturnToMove() const { return m_bNowReturnToHome; }
	
	void RouteMove(vec3& tarPos);
	void RouteMoveStop();
	
private:
	GEntityNPC*				m_pOwner;
	// 비전투 이동시 달릴지 여부
	bool					m_bAlwaysRun;
	// 도망치는 중
	bool					m_bNowRetreat;
	// 전투후 돌아가는 중
	bool					m_bNowReturnToHome;
	
};
