#ifndef _GGUILDSYSTEM_FOR_DBTASK_H
#define _GGUILDSYSTEM_FOR_DBTASK_H

class GDBTaskGuildSerializeData;
class GDBT_GUILD;
class GDBT_GUILD_UPDATE_STORAGEMONEY;

class GGuildSystemForDBTask : public MTestMemPool<GGuildSystemForDBTask>
{
public:
	void GuildSerialize(GDBTaskGuildSerializeData& Data);
	void InsertGuild(GDBT_GUILD& data);
	void DeleteGuild(const int nGID);
	void Join(const MUID& uidJoinMember, const int nGID);
	void Leave(const MUID& uidMember, const int nGID);
	void Kick(int nCID, const int nGID);
	void ChangeMaster(const int nGID, const MUID& uidCurMaster, int nNewMasterCID);	
	void DepositStorageMoney(GDBT_GUILD_UPDATE_STORAGEMONEY& data);
	void WithdrawStorageMoney(GDBT_GUILD_UPDATE_STORAGEMONEY& data);	
};


#endif