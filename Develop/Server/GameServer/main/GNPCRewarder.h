#pragma once

class GEntityNPC;
class GNPCInfo;
class GFieldInfo;

TEST_FORWARD_FT(NPCRewarder, Fixture, DivideContriubtorByServer);
TEST_FORWARD_FT(NPCRewarder, Fixture, Exp);
TEST_FORWARD_FT(NPCRewarder, Fixture, Quest);
TEST_FORWARD_FT(NPCRewarder, Fixture, Faction);
TEST_FORWARD_FT(NPCRewarder, Fixture, ItemByDie);
TEST_FORWARD_FT(NPCRewarder, Fixture, ItemByGather);
TEST_FORWARD_FT(NPCRewarder, Fixture, RewardOtherServerBeneficiary);
TEST_FORWARD_FT(NPCRewarder, Fixture, NotifyReward);
TEST_FORWARD_FT(NPCRewarder, Fixture, DoNotRewardInChallengerField);

class GNPCRewarder : public MTestMemPool<GNPCRewarder>
{
TEST_FRIEND_FT(NPCRewarder, Fixture, DivideContriubtorByServer);
TEST_FRIEND_FT(NPCRewarder, Fixture, Exp);
TEST_FRIEND_FT(NPCRewarder, Fixture, Quest);
TEST_FRIEND_FT(NPCRewarder, Fixture, Faction);
TEST_FRIEND_FT(NPCRewarder, Fixture, ItemByDie);
TEST_FRIEND_FT(NPCRewarder, Fixture, ItemByGather);
TEST_FRIEND_FT(NPCRewarder, Fixture, RewardOtherServerBeneficiary);
TEST_FRIEND_FT(NPCRewarder, Fixture, NotifyReward);
TEST_FRIEND_FT(NPCRewarder, Fixture, DoNotRewardInChallengerField);

public:
	GNPCRewarder();

	void RewardByDie(GEntityNPC* pNPC);
	void RewardByDieForMasterServer(const vector<GEntityPlayer*>& vecSameServerBeneficiary, int nAllBeneficiaryCount, int nNPCID);
	void RewardByGather(GEntityPlayer* pPlayer, int nLootID, GEntityNPC* pNPC);

private:
	void DivideContriubtorByServer(const vector<CID>& vecBeneficiaryCID, vector<GEntityPlayer*>& outvecSameServerBeneficiary, vector<CID>& outvecOtherServerBeneficiaryCID);

	void Exp(const vector<GEntityPlayer*>& vecSameServerBeneficiary, int nAllBeneficiaryCount, GNPCInfo* pNPCInfo);
	void Quest(const vector<GEntityPlayer*>& vecSameServerBeneficiary, GEntityNPC* pNPC);
	void Faction(const vector<GEntityPlayer*>& vecSameServerBeneficiary, GNPCInfo* pNPCInfo);
	void ItemByDie(const vector<CID>& vecBeneficiaryCID, MUID nPartyUID, GEntityNPC* pNPC);
	void ItemByGather(GEntityPlayer* pPlayer, int nLootID, GEntityNPC* pNPC);

	void RewardOtherServerBeneficiary(const vector<CID>& vecOtherServerBeneficiaryCID, int nAllBeneficiaryCount, int nNPCID);

	void NotifyReward(vector<GEntityPlayer*> vecSameServerBeneficiary, int nNPCID);
	void NotifyReward(GEntityPlayer* pSameServerBeneficiary, int nNPCID);

	bool IsChallengerField( const GFieldInfo* pFieldInfo ) const;
};
