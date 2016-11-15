#pragma once

class GEntityPlayer;
class GParty;

class GUTHelper_Party
{
public:
	static GParty* CreateParty(GEntityPlayer* pLeader);
	static GParty* CreateParty(GEntityPlayer* pLeader, GEntityPlayer* pMember);
	static void AddMember(GParty* pParty, GEntityPlayer* pMember);	
	static void LeaveMember(GParty* pParty, GEntityPlayer* pMember);
	static void DeleteParty(GParty* pParty);
	static void DoOfflineMember(GParty* pParty, GEntityPlayer* pMember);
	static void DoOnlineMember(GParty* pParty, GEntityPlayer* pMember);

private:
	static void AddQuest(GParty* pParty, GEntityPlayer* pMember);
	static void RemoveQuest(GParty* pParty, GEntityPlayer* pMember);
	static void AddField(GParty* pParty, GEntityPlayer* pMember);
	static void RemoveField(GParty* pParty, GEntityPlayer* pMember);
	static void SyncQuestRunner(GParty* pParty, GEntityPlayer* pMember);
	static void SyncEraseQuestRunner(GParty* pParty, GEntityPlayer* pMember);
};
