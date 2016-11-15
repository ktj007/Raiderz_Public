#ifndef _G_GUILD_MGR_H_
#define _G_GUILD_MGR_H_

class GGuild;
class GEntityPlayer;
class GGuildStorage;

typedef map<int, GGuild*>				MAP_GUILD;
typedef map<const wchar_t*, GGuild*>	MAP_GUILD_BY_NAME;
typedef map<CID, CID>					MAP_GUILD_INVITE;

class GGuildMgr : public MTestMemPool<GGuildMgr>
{
private:
	MAP_GUILD			m_mapGuild;
	MAP_GUILD_BY_NAME	m_mapGuildByName;
	MAP_GUILD_INVITE	m_mapGuildInvite;	///< 초대한 길드와 초대 받은자

public:
	GGuildMgr();
	~GGuildMgr();

	void Clear();
	
	GGuild* Insert(int nID, const wchar_t* szName, int nMoney=0);
	void Delete(int nID);

	GGuild* Get(int nID);
	GGuildStorage* GetStorage(int nID);
	const wchar_t* GetName(int nID);
	size_t GetSize() { return m_mapGuild.size(); }	

	bool IsExist(int nID);
	bool IsExist(const wchar_t* szName);

	void Invite(CID nGuestCID, const CID nInvitorCID);
	void InviteEnd(CID nGuestCID);
	CID	GetInvitorCID(CID nGuestCID);	
};

#endif
