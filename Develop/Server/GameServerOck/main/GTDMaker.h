#ifndef _G_TRANS_DATA_SYTEM_H_
#define _G_TRANS_DATA_SYTEM_H_

#include "CTransData.h"

class GPlayerObject;
class GEntityPlayer;
struct NPC_ICON;

class GTDMaker
{
public:
	GTDMaker();
	~GTDMaker();

	void MakeTD_MyInfo(GPlayerObject* pPlayer, TD_MYINFO& out);
	void MakeTD_BUFF_REMAIN_TIME(GEntityPlayer* pPlayer, vector<TD_BUFF_REMAIN_TIME>& outvecTDBuffRemainTime);
	void MakeTD_TALENT_REMAIN_COOLTIME(GEntityPlayer* pPlayer, vector<TD_TALENT_REMAIN_COOLTIME>& outvecTDTalentRemainCoolTime);
	void MakeTD_ItemFromInventory(GEntityPlayer* pPlayer, vector<TD_ITEM>& outvecTDItem);
	void MakeTD_ItemFromEquipment(GEntityPlayer* pPlayer, vector<TD_ITEM>& outvecTDItem);
	void MakeTD_ItemFromStorage(GEntityPlayer* pPlayer, vector<TD_ITEM>& outvecTDItem);
	void MakeTD_ReservedDummySlot(GEntityPlayer* pPlayer, vector<TD_ITEM_SLOT_INFO>& outvecTDSlotInfo);
	void MakeTD_Faction(GEntityPlayer* pPlayer, vector<TD_FACTION>& outvecTDFaction);
	vector<TD_NPC_ICON> MakeTD_NPCIcon(const vector<NPC_ICON>& vecNPCICon);
	vector<TD_ESSENTIAL_NPC_ICON> MakeTD_EssentialNPCIcon(const vector<NPC_ICON>& vecNPCICon);
	void MakeTD_PLAYERQUEST(GEntityPlayer* pEntityPlayer, vector<TD_PLAYERQUEST>& outvecTDPlayerQuest);
};

#endif
