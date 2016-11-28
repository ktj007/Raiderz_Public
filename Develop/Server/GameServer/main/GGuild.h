#ifndef _G_GUILD_H_
#define _G_GUILD_H_

#include "CSGuild.h"
#include "GGuildMember.h"
typedef map<CID, GGuildMember*> MAP_GUILD_MEMBER;

class GGuildStorage;

class GGuild : public MTestMemPool<GGuild>
{
friend class GGuildMgr;

private:
	int						m_nID;				///< 길드 ID
	wstring					m_strName;			///< 길드 이름
	CID						m_nMasterCID;		///< 길드 마스터CID
	MAP_GUILD_MEMBER		m_mapMember;		///< 전체 멤버
	
	wstring					m_strMasterName;	///< 길드 마스터 이름(클라에서 CID대신 이름으로 관리해서)

	GGuildStorage*			m_pStorage;			///< 길드 보관함
	int						m_nStorageMoney;	///< 길드 보관함 돈

private:
	GGuild(int nID, const wchar_t* szName, int nMoney=0);	

public:	
	~GGuild();

	void InsertMember(AID nAID, CID nCID, const wchar_t* szName, int nLevel, GUILD_MEMBER_GRADE nGrade);
	void DeleteMember(CID nCID);

	void ChangeGuildMaster(CID nOldMasterCID, CID nNewMasterCID);

	int GetID() { return m_nID; }
	const wchar_t* GetName() { return m_strName.c_str(); }
	CID GetMasterCID() { return m_nMasterCID; }
	const wchar_t* GetMasterName() { return m_strMasterName.c_str(); }
	MAP_GUILD_MEMBER& GetContainer() { return m_mapMember; }

	GGuildMember* GetMember(CID nCID);
	GGuildMember* GetMember(const wchar_t* szName);
	bool IsExist(CID nCID);
	bool IsExist(const wchar_t* szName);
	bool IsGuildMaster(CID nCID);
	bool IsOnlineMember(CID nCID);
	size_t GetMemberCount() { return m_mapMember.size(); }

	vector<CID> CollectMemberCID();

	void Delete();

	void DepositStorageMoney(int nMoney);
	void WithdrawStorageMoney(int nMoney);
	void SetStorageMoney(int nMoney);
	int	 GetStorageMoney();
	GGuildStorage* GetStorage();
};

#endif
