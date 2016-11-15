#pragma once

#include "CTransData.h"
#include "GActorObserver.h"

class GEntityNPC;
struct BPART_REWARD_INFO;

class GNPCBParts : public GActorObserver, public MTestMemPool<GNPCBParts>
{
public:
	GNPCBParts(GEntityNPC* pOwner);
	~GNPCBParts(void);

	// 파츠 부수기
	void Break(uint8 nPartID, MUID uidOpponent);

	// 부서진 파츠 복구
	void Recovery();
	// 업데이트 처리
	void Update(float fDelta);

	// 특정 파츠가 부서졌는가?
	bool IsBroken(uint8 nPartID) const;
	// 파츠 부서짐 설정
	void SetBreak(uint8 nPartID);
	// 파츠가 부서진게 있는가?
	bool IsAnyPartBroken() const;

	void MakeTD(TD_UPDATE_CACHE_NPC& out) const;
private:
	void RouteBreak( MUID uidOpponent, uint8 nPartID, const vector<TD_USABLE_BPART>& vecUsableBParts );
	void RouteRecovery();
	vector<BPART_REWARD_INFO> GetRewardInfo( uint8 nPartID );
	void OnReward(const vector<BPART_REWARD_INFO>& vecRewardInfo, MUID uidRewarder, vector<TD_USABLE_BPART>& outUsableBParts, vector<GEntityNPC*>& outBPartNPCs);
	void RouteInsertLoobatleNPC( const vector<GEntityNPC*>& outBPartNPCs );
private:
	// 자신이 죽었을때
	virtual void OnDie() override;

private:
	GEntityNPC* m_pOwner;
	set<CHAR>	m_setBrokenParts;
};
