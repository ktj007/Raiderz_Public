#ifndef _GDBTASK_GUILD_SERIALIZE_H
#define _GDBTASK_GUILD_SERIALIZE_H


#include "GDBAsyncTask.h"
#include "MMemPool.h"
#include "GDBTaskItem.h"


struct GDBTASK_GUILD_MEMBER
{
	int64				nAID;
	int64				nCID;
	wstring				strCharName;
	uint8				nLevel;
	uint8				nGuildGrade;
};

typedef vector<GDBTASK_GUILD_MEMBER> GDBT_GUILD_MEMBER_VEC;


class GDBTaskGuildSerializeData
{
public :
	MUID					uidPlayer;
	int64					nGID;
	wstring					strGuildName;
	GDBT_GUILD_MEMBER_VEC	vecGuildMember;
	int						nStorageMoney;
	GDBT_ITEMINSTANCEQ		qItemInst;
};


class GDBTaskGuildSerialize : public GDBAsyncTask, public MMemPool<GDBTaskGuildSerialize>
{
public :
	GDBTaskGuildSerialize(const MUID& uidReqPlayer);
	~GDBTaskGuildSerialize();

	enum
	{
		GUILD_INFO = 0
		, GUILD_MEMBERLIST
		, GUILD_STORAGEITEMLIST
	};

	void					Input(const int64 nGID);
	bool					GetGuildInfo(mdb::MDatabase& rfDB);
	bool					GetGuildMemberList(mdb::MDatabase& rfDB);
	bool					GetGuildStorageItemList(mdb::MDatabase& rfDB);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;
	
	
private :
	class Completer
	{
	public :
		Completer(GDBTaskGuildSerializeData& Data);
		void Do();

	private :
		GDBTaskGuildSerializeData& m_Data;
	};

protected :
	GDBTaskGuildSerializeData m_Data;
};

#endif