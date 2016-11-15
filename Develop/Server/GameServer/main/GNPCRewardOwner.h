#pragma once

#include "GActorObserver.h"
#include "GKillRewardOwner.h"

class GEntityNPC;

TEST_FORWARD_FT(NPCRewardOwner, Fixture, OnAddHateEntry);
TEST_FORWARD_FT(NPCRewardOwner, Fixture, OnAddHateEntryMember);
TEST_FORWARD_FT(NPCRewardOwner, Fixture, OnAddHateEntryMember);
TEST_FORWARD_FT(NPCRewardOwner, Fixture, OnAddHateEntry_MaintainRewardOwner);
TEST_FORWARD_FT(NPCRewardOwner, Fixture, OnLostHateEntry_PartPartyMember);
TEST_FORWARD_FT(NPCRewardOwner, Fixture, OnLostHateEntry_AllPartyMember);
TEST_FORWARD_FT(NPCRewardOwner, Fixture, OnLostHateEntry_ChangeRewardOwner);
TEST_FORWARD_FT(NPCRewardOwner, Fixture, OnEmptyHateTable);

class GNPCRewardOwner : private GActorObserver, public GKillRewardOwner, public MTestMemPool<GNPCRewardOwner>
{
TEST_FRIEND_FT(NPCRewardOwner, Fixture, OnAddHateEntry);
TEST_FRIEND_FT(NPCRewardOwner, Fixture, OnAddHateEntryMember);
TEST_FRIEND_FT(NPCRewardOwner, Fixture, OnAddHateEntryMember);
TEST_FRIEND_FT(NPCRewardOwner, Fixture, OnAddHateEntry_MaintainRewardOwner);
TEST_FRIEND_FT(NPCRewardOwner, Fixture, OnLostHateEntry_PartPartyMember);
TEST_FRIEND_FT(NPCRewardOwner, Fixture, OnLostHateEntry_AllPartyMember);
TEST_FRIEND_FT(NPCRewardOwner, Fixture, OnLostHateEntry_ChangeRewardOwner);
TEST_FRIEND_FT(NPCRewardOwner, Fixture, OnEmptyHateTable);


public:
	GNPCRewardOwner(GEntityNPC* pOwner);
	virtual ~GNPCRewardOwner(void);

private:
	virtual void OnAddHateEntry(GEntityActor* pEnemy) override;
	virtual void OnAddHateEntryMember(GEntityActor* pEnemyMember) override;
	virtual void OnLostHateEntry(MUID nEnemty) override;
	virtual void OnEmptyHateTable() override;
	
private:
	vector<int> SelectDeletableCandidateCID(GEntityPlayer* pPlayer);	

private:
	GEntityNPC*	m_pOwner;
};
