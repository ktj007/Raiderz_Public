#pragma once

#include "MMemPool.h"

class GEntityNPC;

class GNPCAssist : public MMemPool<GNPCAssist>
{
public:
	GNPCAssist(GEntityNPC* pOwner);
	~GNPCAssist(void);

	void Update(float fDelta);

	void RequestAssistanceToNeighbors();
	void OnYell(GEntityNPC* pTarget);

private:
	float GetAssistRange() const;
private:
	GEntityNPC* m_pOwner;
	MRegulator	m_rgrTick;
};

/// GNPCAssist를 도와주는 헬퍼 클래스
class GNPCAssistHelper
{
public:
	static bool IsAssistable(GEntityNPC* pNPC);
	static bool IsRequestable(GEntityNPC* pNPC);

	static float GetAssistRange(GEntityNPC* pNPC);

	static bool CheckFaction(GEntityNPC* pAssistor, GEntityNPC* pTarget);
	static bool CheckRange(GEntityNPC* pAssistor, GEntityNPC* pTarget);
};

