#pragma once

enum CONDITION_TYPE;
enum NPC_ICON_TYPE;

class GEntityPlayer;
class GEntityNPC;
class GVectorMUID;
class GNPCIconSelector;

struct NPC_ICON;

TEST_FORWARD_FT(PlayerNPCIconSender, Fixture, CollectEssentialNPCUID);
TEST_FORWARD_FT(PlayerNPCIconSender, Fixture, FilterSendedIcon);
class GPlayerNPCIconSender : public MTestMemPool<GPlayerNPCIconSender>
{
TEST_FRIEND_FT(PlayerNPCIconSender, Fixture, CollectEssentialNPCUID);
TEST_FRIEND_FT(PlayerNPCIconSender, Fixture, FilterSendedIcon);

public:
	GPlayerNPCIconSender(GEntityPlayer* pOwner);
	~GPlayerNPCIconSender(void);

	void SendByFindEnemy(GEntityNPC* pNPC);
	void SendByPlayerConditionChange(CONDITION_TYPE nConditionType);
	void SendByPlayerIn(const vector<MUID>& vecSetcorNPCUID, bool bFieldChange);
	void SendByNPC(GEntityNPC* pNPC);
	void SendByDisableNPCInteraction(GEntityNPC* pNPC);
	void RemoveNPCIconData(const vector<MUID>& vecNPCUID);
	void ClearNPCIconData();

private:
	void Select(const vector<MUID>& vecNPCUID, vector<NPC_ICON>& vecNPCIcon, CONDITION_TYPE nConditionType, const vector<MUID>& vecEssentialNPCUID=vector<MUID>(), vector<NPC_ICON>& vecEssentialNPCIcon=vector<NPC_ICON>());
	void Send(const vector<NPC_ICON>& vecNPCIcon, const vector<NPC_ICON>& vecEssentialNPCIcon=vector<NPC_ICON>());
	void CollectEssentialNPCUID(vector<MUID>& vecNPCUID);
	void FilterSendedIcon(const vector<NPC_ICON>& vecNPCIcon, vector<NPC_ICON>& vecFilteredNPCIcon);
	void FilterEssentialNPCIcon(const vector<NPC_ICON>& vecNPCIcon, vector<NPC_ICON>& vecFilteredNPCIcon);	

private:
	GEntityPlayer*				m_pOwner;
	GNPCIconSelector*			m_pIconSelector;
	map<MUID, NPC_ICON_TYPE>	m_mapSendedNPCIcon;
};