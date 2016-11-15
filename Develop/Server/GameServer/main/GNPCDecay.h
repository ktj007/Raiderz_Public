#ifndef _G_NPC_DECAY_H_
#define _G_NPC_DECAY_H_

class GEntityNPC;
class MRegulator;

class GNPCDecay : public MTestMemPool<GNPCDecay>
{
private:
	GEntityNPC* m_pOwner;
	MRegulator*	m_pDecayRegulator;		///< 소멸될때까지 걸리는 시간
	float		m_fCustomDecayTick;		///< 기본값이 아닌 특정 이벤트를 위해서 사용.
										///< ex) 퀘스트 NPC같은 경우는 기본값보다 더 긴시간 시체가 남아 있어야 한다.
private:
	void _Decay(float fDecayInterval);

public:
	GNPCDecay(GEntityNPC* pOwner);	
	~GNPCDecay();
	
	void Decay();

	void RemoveToSpawnMgr();
	void Update(float fDelta);
	void SetCustomDecayTick(const float fTick);
};

#endif
