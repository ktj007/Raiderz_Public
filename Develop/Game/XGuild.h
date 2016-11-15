#ifndef _X_GUILD_H_
#define _X_GUILD_H_

#include "CSGuild.h"
#include "XMyStorage.h"

class XGuildMember;

typedef map<wstring, XGuildMember*> MAP_GUILD_MEMBER;

class XGuild
{
private:
	int					m_nStorageMoney;

public:
	wstring				m_strName;			///< ±æµå¸í
	wstring				m_strMasterName;
	MAP_GUILD_MEMBER	m_mapMember;

public:
	XGuild();
	~XGuild();

	void Clear();

	void InsertMember(const wchar_t* szName, int nLevel, GUILD_MEMBER_GRADE nGrade, bool bOnline, int nFieldID, int nChannelID);
	void DeleteMember(const wchar_t* szName);

	void OnLine(const wchar_t* szName, int nFieldID, int nChanneldID);
	void OffLine(const wchar_t* szName);
	void MoveField(const wchar_t* szName, int nFieldID, int nChanneldID);

	void ChangeGuildMaster(const wchar_t* szOldMasterName, const wchar_t* szNewMasterName);

	XGuildMember* GetMember(const wchar_t* szName);
	bool GetIndex( size_t nIndex, XGuildMember** ppOutMember);

	bool IsExist();

	wstring GetMemberList();

	void SetStorageMoney( int nMoney );
	int GetStorageMoney()	{ return m_nStorageMoney;	}
};
 
#endif
