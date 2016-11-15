#ifndef _G_GUILD_MEMBER_H_
#define _G_GUILD_MEMBER_H_

#include "CSGuild.h"

class GGuildMember : public MTestMemPool<GGuildMember>
{
private:
	int64				m_nAID;
	int					m_nCID;
	wstring				m_strName;
	int					m_nLevel;
	GUILD_MEMBER_GRADE	m_nGrade;
	bool				m_bOnline;
	int					m_nFieldID;
	int					m_nChannelID;

public:
	GGuildMember(int64, int nCID, const wchar_t* szName, int nLevel, GUILD_MEMBER_GRADE nGrade);
	~GGuildMember();

	void OnLine(int nFieldID, int nChannelID);
	void OffLine();
	void MoveField(int nFieldID, int nChannelID);

	void ChangeGrade(GUILD_MEMBER_GRADE nGrade);

	int64 GetAID() { return m_nAID; }
	int GetCID() { return m_nCID; }
	
	const wchar_t* GetName() { return m_strName.c_str(); }
	int GetLevel() { return m_nLevel; }
	GUILD_MEMBER_GRADE GetGrade() { return m_nGrade; }

	bool IsOnline();
	int GetFieldID();
	int GetChannelID();
};

#endif
